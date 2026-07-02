
#include "MainCMDInterpreter.h"
#include "../ExecManager.h"
#include "../stexcept.h"
#include "../../inter/InterManager.h"
#include "../../darv/MainScript.h"
#include "../../darv/CMD.h"
#include "../../io/io.h"
#include "../../util/strutil.h"
#include "../../shell/shell.h"
#include "../../msg/messagebuilder.h"

#include "../../consts.h"
#include "../../error_messages.h"
#include "../../info_messages.h"

#include <iostream>
using std::endl;

/*
Classe responsável por ler e setar workingDir e scriptFile, interpretar 
o script principal, carregando toda a arvore de derivação, carregar as 
variáveis e propriedades definidas no comando principal com "-var" e "-prop" e 
validar o comando.

Para validar o comando principal, é verificada cada tarefa informada no comando. Se, após 
a interpretação do script e geração da arvore de derivação, alguma tarefa informada 
no comando principal não for, nem tarefa default, nem estiver definida no script, então a 
validação falha.

Exemplo:
    Informada a tarefa "executar". Ela não é uma tarefa default executada pelo sistema e, 
    caso não tenha sido definida no arquivo de configurações, também não é uma tarefa 
    de usuário. Logo, é uma tarefa desconhecida e o sistema informa isso para o usuário.
*/

void MainCMDInterpreter::configureAndInterpretsAndValidate( void* mgr ) {
    bool workingDirFound = false;
    bool scriptFileFound = false;

    this->configure( workingDirFound, scriptFileFound, mgr );
    this->interpretsMainScript( workingDirFound, scriptFileFound, mgr );    
    this->validaMainCMD( mgr );
}

void MainCMDInterpreter::configure( bool& workingDirFound, bool& scriptFileFound, void* mgr ) {
    ExecManager* manager = (ExecManager*)mgr;
    ExecCMD* mainExecCMD = manager->getMainExecCMD();

    Output& out = manager->out;

    workingDir = mainExecCMD->getPropertyValue( "--working-dir" );
    scriptFile = mainExecCMD->getPropertyValue( "--script" );

    if ( workingDir != "" ) {
        workingDir = io::path::absoluteResolvePath( workingDir );
        workingDir = io::path::removeSeparatorFromDirIfNeed( workingDir );

        if ( !io::fileExists( workingDir ) ) {
            messagebuilder b( errors::WORKING_DIR_NOT_FOUND );
            b << workingDir;
            throw st_error( nullptr, b.str() );
        }

        shell::setWorkingDir( workingDir );

        workingDirFound = true;
    } else {
        if ( scriptFile != "" ) {
            workingDir = io::path::dirPath( io::path::absoluteResolvePath( scriptFile ) );
            workingDir = io::path::removeSeparatorFromDirIfNeed( workingDir );

            if ( !io::fileExists( workingDir ) ) {
                messagebuilder b( errors::WORKING_DIR_NOT_FOUND );
                b << workingDir;
                throw st_error( nullptr, b.str() );
            }

            scriptFile = io::path::fileOrDirName( scriptFile );
            shell::setWorkingDir( workingDir );
        } else {
            workingDir = shell::getWorkingDir();
        }

        workingDirFound = false;
    }

    if ( scriptFile == "" )
        scriptFile = consts::DEFAULT_SCRIPT_FILE_NAME;

    scriptFile = io::path::absoluteResolvePath( scriptFile );

    if ( !io::fileExists( scriptFile ) && io::fileExists( scriptFile+".txt" ) )
        scriptFile += ".txt";
   
    scriptFileFound = true;
    
    if ( !io::fileExists( scriptFile ) && !io::fileExists( scriptFile+".txt" ) ) {
        messagebuilder b2( errors::SCRIPT_FILE_NOT_FOUND );
        b2 << scriptFile;
        out << output::yellow( b2.str() ) << endl;

        if ( !workingDirFound )
            throw st_error( nullptr, errors::NO_SCRIPT_FILE_AND_NO_WORKING_DIR );

        scriptFileFound = false;
    }
}

void MainCMDInterpreter::interpretsMainScript( bool workingDirFound, bool scriptFileFound, void* mgr ) {
    ExecManager* manager = (ExecManager*)mgr;
    InterManager* interManager = manager->getInterManager();
    MainScript* mainScript = manager->getMainScript();
    ScriptPropertyManager* scriptPropManager = manager->getScriptPropManager();    

    this->loadProperties( mgr );
    this->loadVariables( mgr );

    string os = "nowindows";
    #ifdef _WIN32
        os = "windows";
    #else
        os = "nowindows";
    #endif

    mainScript->putPredefinedVar( "os", os );
    mainScript->putPredefinedVar( "script_file", scriptFile );
    mainScript->putPredefinedVar( "working_dir", workingDir );

    if ( scriptFileFound ) {
        InterResult* result = interManager->interpretsMainScript( mainScript, scriptFile );
        if ( !result->isInterpreted() )
            throw st_error( result );

        delete result;
    }

    string basedir = scriptPropManager->getBaseDir();
    if ( basedir != "" ) {
        basedir = io::path::absoluteResolvePath( basedir );
        if ( !io::fileExists( basedir ) ) {
            messagebuilder b( errors::BASE_DIRECTORY_NOT_FOUND );
            b << basedir << props::BASE_DIR;
            throw st_error( nullptr, b.str() );
        }
        
        shell::setWorkingDir( basedir );

        workingDir = basedir;
    }
}

void MainCMDInterpreter::validaMainCMD( void* mgr ) {
    ExecManager* manager = (ExecManager*)mgr;
    MainScript* script = manager->getMainScript();
    
    ExecCMD* mainExecCMD = manager->getMainExecCMD();
    CMD* mainCMD = mainExecCMD->getCMD();

    vector<string>& args = mainExecCMD->getArgs();
    int len = args.size();
    for( int i = 0; i < len; i++ ) {
        string arg = args[ i ];
        if ( strutil::startsWith( arg, "-" ) )
            continue;

        if ( i > 0 )
            if ( args[ i-1 ] == "-var" || args[ i-1 ] == "-prop" )
                continue;
        
        bool isDefaultTask = manager->isDefaultTask( arg );
        bool isUserTask = script->existsTask( arg );

        if ( !isDefaultTask && !isUserTask ) {
            messagebuilder b( errors::CMD_TASK_NOT_FOUND );
            b << arg;
            throw st_error( mainCMD, b.str() );
        }
    }
}

void MainCMDInterpreter::loadProperties( void* mgr ) {
    ExecManager* manager = (ExecManager*)mgr;
    MainScript* mainScript = manager->getMainScript();

    ExecCMD* mainExecCMD = manager->getMainExecCMD();
    CMD* mainCMD = mainExecCMD->getCMD();

    vector<string> properties = mainExecCMD->getOpArgValues( "-prop" );

    for( string prop : properties ) {
        size_t i = prop.find( '=' );
        if ( i == string::npos ) {
            messagebuilder b( errors::INVALID_PROP_DEF );
            b << prop;
            throw st_error( mainCMD, b.str() );
        }

        string propName = prop.substr( 0, i );
        string propValue = prop.substr( i+1, prop.length()-i-1 );

        if ( !manager->isValidProp( propName ) ) {
            messagebuilder b( errors::IS_NOT_A_VALID_PROP );
            b << propName;
            throw st_error( mainCMD, b.str() );
        }

        mainScript->putProperty( propName, propValue );
    }
}

void MainCMDInterpreter::loadVariables( void* mgr ) {
    ExecManager* manager = (ExecManager*)mgr;
    MainScript* mainScript = manager->getMainScript();
    ExecCMD* mainExecCMD = manager->getMainExecCMD();
    CMD* mainCMD = mainExecCMD->getCMD();

    vector<string> variables = mainExecCMD->getOpArgValues( "-var" );

    for( string var : variables ) {
        size_t i = var.find( '=' );
        if ( i == string::npos ) {
            messagebuilder b( errors::INVALID_VAR_DEF );
            b << var;
            throw st_error( mainCMD, b.str() );
        }

        string varName = var.substr( 0, i );
        string varValue = var.substr( i+1, var.length()-i-1 );

        mainScript->putLocalVar( varName, varValue );
    }
}

string MainCMDInterpreter::getScriptFile() {
    return scriptFile;
}

string MainCMDInterpreter::getWorkingDir() {
    return workingDir;
}
