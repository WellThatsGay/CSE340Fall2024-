#include <string>
#include "lexer.h"

/*Christopher Harris*/
/*Project 3*/
/*CSE 340*/

class Parser {
private:
    LexicalAnalyzer lexer;

    // Handles syntax errors encountered during parsing
    void syntax_error();

    // Checks if the next token matches the expected type
    Token expect(TokenType expected_type);

public:
    // Entry point for parsing an entire program
    void parse_program();

    // Parses variable section
    void parse_var_sction();

    // Parses a list of identifiers
    void parse_id_list();

    // Parses the main body of the program
    void parse_body();

    // Parses a list of statements
    void parse_stmt_list();

    // Parses an individual statement
    void parse_stmt();

    // Parses an assignment statement
    void parse_assign_stmt();

    // Parses an expression
    void parse_expr();

    // Parses a primary expression
    void parse_primary();

    // Parses an operator
    void parse_op();

    // Parses an output statement
    void parse_output_stmt();

    // Parses an input statement
    void parse_input_stmt();

    // Parses a while loop statement
    void parse_while_stmt();

    // Parses an if statement
    void parse_if_stmt();

    // Parses a condition
    void parse_condition();

    // Parses a relational operator
    void parse_relop();

    // Parses a switch statement
    void parse_switch_stmt();

    // Parses a for loop statement
    void parse_for_stmt();

    // Parses a list of cases in a switch statement
    void parse_case_list();

    // Parses an individual case in a switch statement
    void parse_case();

    // Parses the default case in a switch statement
    void parse_default_case();

    // Parses input statements
    void parse_inputs();

    // Parses a list of numbers
    void parse_num_list();
};