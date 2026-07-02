#ifndef ERROR_MESSAGE_H
#define ERROR_MESSAGE_H

#include <string>

using std::string;

namespace errors {

    const string ARCHIVING_FAILED = "Falha no archiving!";
    const string COMPILING_FAILED = "Falha na compilação!";
    const string LINKING_FAILED = "Falha na linkagem!";
    const string TEST_LINKING_FAILED = "Falha na linkagem do executável de teste!";
    const string TESTING_FAILED = "Falha na execução da tarefa teste!";

    const string NO_SCRIPT_FILE_AND_NO_WORKING_DIR = "Como o arquivo de script não foi encontrado ou informado, é necessario definir o argumento --working-dir";

    const string SRC_DIRECTORY_NOT_FOUND = "Diretório de códigos fonte não encontrado: \"$1\"\nVerifique a propriedade \"$2\"";
    const string WORKDIR_VAR_NOT_FOUND = "Não foi encontrada a variável de diretório de trabalho.";

    const string INVALID_NUMBER_OF_ARGS = "Número de argumentos esperado igual a $1, encontrado $2.";

    const string INVALID_PROP_DEF = "Definicao de propriedade inválida: \"$1\"";
    const string INVALID_VAR_DEF = "Definicao de variável inválida: \"$1\"";

    const string IS_NOT_A_VALID_PROP = "A propriedade \"$1\" não foi reconhecida.";
    const string IS_NOT_A_DEFAULT_TASK = "A tarefa \"$1\" não é uma tarefa default. Logo, não pode ser configurada assim.";

    const string SCRIPT_FILE_NOT_FOUND = "Arquivo de script não encontrado: \"$1\"";
    const string FILE_OR_DIRECTORY_NOT_CREATED = "Não foi possível criar o diretório: \"$1\"";
    const string FILE_OR_DIRECTORY_NOT_COPIED_FOR_BUILD_FOLDER = "não foi possível cópiar o arquivo ou pasta: \"$1\" para a pasta de build.";
    const string FILE_OR_FOLDER_NOT_FOUND = "Arquivo ou diretório não encontrado: \"$1\"";
    const string FILE_OR_FOLDER_NOT_DELETED = "Arquivo ou pasta não deletado: \"$1\"\nSe o arquivo é um diretório, verifique se está vasio ou tente remover recursivamente.";
    const string FILE_OR_FOLDER_DELETION_IS_NOT_POSSIBLE = "Não foi possível deletar o arquivo ou pasta: \"$1\"";
    const string RECURSIVE_FILE_OR_FOLDER_NOT_DELETED = "Arquivo ou pasta não deletado: \"$1\"";
    const string RESOURCE_FILE_NOT_EXISTS = "O arquivo de recursos de ícone: \"$1\" não foi encontrado.";

    const string FOLDER_ALREADY_EXISTS = "Pasta já existe: \"$1\"";
    const string TRY_RECURSIVE_COPY_OF_NOT_EMPTY_FOLDER = "Tentativa de cópiar não recursivamente uma pasta não vazia.";
    const string TRY_COPY_DIR_TO_FILE = "Tentativa de copiar uma pasta para um arquivo.\norigem=\"$1\"\ndestino=\"$2\"";
    const string TRY_DELETE_NO_RECURSIVE_NO_EMPTH_DIR = "Tetativa de remover arquivos de forma não recursiva com pasta(s) não vazia(s).";

    const string WORKING_DIR_NOT_FOUND = "Diretório de trabalho não encontrado: \"$1\"";
    const string WORKING_DIR_IS_NOT_A_DIRECTORY = "O diretório de trabalho informado não é um diretório: \"$1\"";
    const string BASE_DIRECTORY_NOT_FOUND = "Diretório base não encontrado: \"$1\"\nVerifique a propriedade \"$2\"";
    const string SRC_DIRECTORY_NOT_EXISTS = "O diretório de origem não existe: \"$1\"";
    const string DEST_DIRECTORY_NOT_EXISTS = "O diretório de destino não existe: \"$1\"";
    const string DIRECTORY_NOT_FOUND = "Diretório não encontrado.";
    const string DIRECTORY_NOT_CREATED = "Não foi possível criar o diretório: \"$1\"";
    const string DIRECTORY_NOT_CREATED_2 = "Não foi possível criar o diretório. \nVerifique se o diretório onde quer criar o novo diretório existe.";
    const string DIRECTORIES_NOT_CREATED = "Não foi possível criar os diretórios do caminho informado.";
    const string CURRENT_DIRECTORY_NOT_CHANGED = "Não foi possível alterar o diretório corrente.";
    const string CURRENT_DIRECTORY_NOT_GET = "Não foi possível capturar o diretório corrente.";
    const string INFORMED_PATH_IS_NOT_A_DIRECTORY = "O caminho informado não é um diretório.";

    const string BEFORE_AND_AFTER_TASK = "A tarefa \"$1\" está sendo definida como before e after.";
    const string CMD_TASK_NOT_FOUND = "A tarefa \"$1\" não existe. Use a opção \"-h\" ou \"--help\" para obter ajuda.";
    const string DEFAULT_USER_TASK_DEFINED_HOW_NORMAL = "A tarefa \"$1\" é default. Por isso, não pode ser definida sem before ou after";
    const string INVALID_TASK_FLAG = "Flag de tarefa inválido: \"$1\"";
    const string NOTHING_TASK_INFORMED = "E necessario informar ao menos uma tarefa como argumento.";
    const string TASK_AFTER_ALREADY_DEFINED = "Já foi definida antes a tarefa \"$1\" como after.";
    const string TASK_BEFORE_ALREADY_DEFINED = "Já foi definida antes a tarefa \"$1\" como before.";
    const string TASK_NAME_NOT_DEFINED = "Nome de tarefa não definido.";
    const string TASK_NOT_RECOGNIZED = "Tarefa não reconhecida.";

    const string PROC_NAME_NOT_INFORMED = "O nome do procedimento não foi informado.";
    const string PROC_NOT_FOUND = "Nenhum procedimento encontrado pelo nome: $1.";

    const string PROPERTY_NOT_DEFINED = "A propriedade \"$1\" deve ser definida.";
    const string PROPERTY_NOT_DEFINED_FOR_ARCHIVING = "A propriedade \"$1\" deve ter valor definido para o empacotamento da biblioteca estática.";
    const string PROPERTY_NOT_DEFINED_FOR_LINKING = "A propriedade \"$1\" deve ter valor definido para linkagem.";
    const string PROP_OR_VAR_NOT_FOUND = "Propriedade ou variável não encontrada: $($1).";
    const string VAR_ATTR_TO_PREDEFINED_VAR = "Tentativa de atribuir valor a variável predefinida: \"$1\"";
    const string VERIFY_THE_PROPERTY = "Verifique a propriedade: \"$1\"";
    
    const string START_OF_BLOCK_WITH_UNNECESSARY_TOKEN = "Início de bloco com caracter(es) desnecessário(s).";

    const string END_OF_SHELL_BLOCK_NOT_FOUND = "Fim de bloco de shell \"endshell\" não encontrado.";
    const string END_OF_TASK_BLOCK_NOT_FOUND = "Fim de bloco de tarefa \"endtask\" não encontrado.";
    const string END_OF_PROC_BLOCK_NOT_FOUND = "Fim de bloco de procedimento \"endproc\" não encontrado.";
    const string END_OF_BLOCK_NOT_FOUND = "Fim de bloco não encontrado.";

    const string IF_WITHOUT_THEN = "Encontrado definição de if sem then.";
    const string IF_CONDITION_WITHOUT_COMP_OPERATOR = "Encontrada condição de if sem operador de comparação.";
    const string IF_OPERAND1_WITHOUT_CLOSE_QUOTES = "Operando 1 de IF sem fecha aspas ou com fecha aspas antes do final.";
    const string IF_OPERAND2_WITHOUT_CLOSE_QUOTES = "Operando 2 de IF sem fecha aspas ou com fecha aspas antes do final.";
    const string IF_UNNECESSARY_ELSE = "Else desnecessário.";
    const string IF_CONDITION_EXPECTED = "Condição de if esperada.";
    const string ENDIF_NOT_FOUND = "if sem endif correspondente.";

    const string PROPERTY_NOT_FOUND = "Propriedade não reconhecida: \"$1\".";
    const string TRY_CHANGE_PREDEFINED_VAR = "Tentativa de alterar variável predefinida: \"$1\".";

    const string VALUE_IN_QUOTES_NOT_CLOSED = "Valor com aspas duplas sem fechar.";
    
    const string ERROR_IN_FILES_COPY = "Houve erro na cópia do(s) arquivo(s).\nVerifique os caminhos da origem e do destino, \nse a cópia é recursiva e com ou sem sobrescrita, \ne se tem permissoes necessarias para está operacao";
    const string ERROR_IN_MAIN_CMD = "Erro: $1";
    const string ERROR_IN_READING_SRC_FILES = "Houve algum erro na leitura dos arquivos de código fonte.";
    const string ERROR_IN_READING_TEST_FILES = "Houve algum erro na leitura dos arquivos de código fonte de teste.";
    
    const string SHELL_CMD_NOT_EXECUTED = "Não foi possível executar o comando.\nVerifique se ele existe e sua sintáxe!\nO comando retornou o código: $1";

    const string UNRECOGNIZED_LINE = "Linha não reconhecida como instrução válida.";

    const string UNNECESSARY_TOKEN = "Token desnecessário: $1";

    const string TEST_FILE_EXE_NOT_FOUND = "O arquivo executável de teste não foi encontrado.\nVerifique se executou a tarefa \"link\" antes da tarefa \"test\".";

namespace io {

    const string FILE_OR_FOLDER_NOT_FOUND = "O arquivo ou pasta não existe.";
    const string FILE_NOT_FOUND = "O arquivo não existe.";
    const string DIR_NOT_FOUND = "A pasta não existe.";

    const string IS_NOT_A_DIR = "Não é um diretório.";
    const string FOLDER_NOT_EMPTY = "Pasta não vazia.";

    const string FOLDER_WITH_NOT_EMPTY_FOLDER = "A pasta contém pasta(s) não vazia(s).";

    const string SRC_DIR_NOT_FOUND = "O diretório de origem não existe.";
    const string TARGET_DIR_NOT_FOUND = "O diretório alvo não existe.";

    const string SRC_IS_NOT_A_REGULAR_FILE = "A origem não é um arquivo regular.";
    const string TARGET_IS_NOT_A_REGULAR_FILE = "O alvo não é um arquivo regular.";

    const string SRC_IS_NOT_A_DIR = "A origem não é um diretório.";
    const string TARGET_IS_NOT_A_DIR = "O alvo não é um diretório.";

}

namespace runtime {

    const string NULL_ROOT_STATEMENT = "A raiz da árvore é nula.";
    const string NULL_PARENT = "O objeto bloco é nulo.";
    const string NULL_PROC = "Proc nula.";
    const string NULL_IF_CONDITION = "IF Condition nula.";

    const string NULL_WORKING_DIR_PRED_VAR = "A variável predefinida working_dir não foi definida.";

    const string CMD_EXECUTOR_NOT_FOUND = "Executor de comando não encontrado pelo nome: \"$1\"";
    const string TASK_EXECUTOR_NOT_FOUND = "Executor de tarefa não encontrado pelo nome: \"$1\"";
    
    const string INVALID_STATEMENT_TYPE = "Instrução de tipo inválido.\nLinha=\"$1\"\nTipo=\"$2\"";
    const string INVALID_STATUS_OF_TASK_CONFIG_INTER = "Status de TaskConfigInter não reconhecido.";

}

}

#endif
