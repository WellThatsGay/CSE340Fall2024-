#include <iostream>
#include "parser.h"
#include "lexer.h"

using namespace std;

/*Christopher Harris*/
/*Project 3*/
/*CSE 340*/


void Parser::syntax_error() {
    // Print an error message and exit the program
    std::cout << "Epsilon is not a token !!!\n";
    exit(1);
}

Token Parser::expect(TokenType expected_type) {
    // Get the next token from the lexer
    Token t = lexer.GetToken();
    
    // Check if the token type matches the expected type
    if (t.token_type != expected_type) {
        syntax_error();
    }
    
    // Return the token if it matches the expected type
    return t;
}

void Parser::parse_program() {
    // Parse the variable section of the program
    parse_var_sction();
    
    // Parse the main body of the program
    parse_body();
    
    // Parse the input statements
    parse_inputs();
    
    // Expect the end of file token
    expect(END_OF_FILE);
}

void Parser::parse_var_sction() {
    // Parse a list of identifiers
    parse_id_list();
    
    // Expect a semicolon after the list of identifiers
    expect(SEMICOLON);
}

void Parser::parse_id_list() {
    // Get the next token from the lexer
    Token t = lexer.GetToken();
    
    // Check if the token type is an identifier
    if (t.token_type != ID) {
        syntax_error();
    }

    // If the next token is a comma, continue parsing the identifier list
    if (lexer.peek(1).token_type == COMMA) {
        lexer.GetToken(); // Consume the comma
        parse_id_list();
    }
}

void Parser::parse_body() {
    // Expect a left brace to start the body
    expect(LBRACE);
    
    // Parse a list of statements within the body
    parse_stmt_list();
    
    // Expect a right brace to end the body
    expect(RBRACE);
}

void Parser::parse_stmt_list() {
    // Parse an individual statement
    parse_stmt();

    // Peek at the next token to determine if there are more statements
    Token t = lexer.peek(1);
    
    // If the next token indicates the start of another statement, continue parsing the statement list
    if (t.token_type == ID || t.token_type == WHILE || t.token_type == IF || t.token_type == SWITCH || t.token_type == FOR || t.token_type == OUTPUT || t.token_type == INPUT) {
        parse_stmt_list();
    }
}

void Parser::parse_stmt() {
    // Peek at the next token to determine the type of statement
    Token t = lexer.peek(1);

    // Use a switch statement to handle different types of statements
    switch (t.token_type) {
        case ID:
            parse_assign_stmt();
            break;
        case WHILE:
            parse_while_stmt();
            break;
        case IF:
            parse_if_stmt();
            break;
        case SWITCH:
            parse_switch_stmt();
            break;
        case FOR:
            parse_for_stmt();
            break;
        case OUTPUT:
            parse_output_stmt();
            break;
        case INPUT:
            parse_input_stmt();
            break;
        default:
            syntax_error();
            break;
    }
}

void Parser::parse_assign_stmt() {
    // Peek at the next token to ensure it is an identifier
    Token t = lexer.peek(1);
    if (t.token_type != ID) {
        syntax_error();
    }

    // Consume the identifier token
    t = lexer.GetToken();
    
    // Ensure the next token is an equal sign
    if (t.token_type != EQUAL) {
        syntax_error();
    }

    // Consume the equal sign token
    t = lexer.GetToken();
    
    // Check if the next token is an operator, indicating an expression
    if (lexer.peek(1).token_type == PLUS || lexer.peek(1).token_type == MINUS || lexer.peek(1).token_type == MULT || lexer.peek(1).token_type == DIV) {
        parse_expr();
    } else {
        parse_primary();
    }
    
    // Expect a semicolon at the end of the assignment statement
    expect(SEMICOLON);
}

void Parser::parse_expr() {
	parse_primary();
	parse_op();
	parse_primary();
}

void Parser::parse_primary() {
    // Get the next token from the lexer
    Token t = lexer.GetToken();
    
    // Check if the token is an identifier or a number
    if (t.token_type == ID || t.token_type == NUM) {
        return;
    } else {
        syntax_error();
    }
}

void Parser::parse_op() {
    // Get the next token from the lexer
    Token t = lexer.GetToken();
    
    // Check if the token is a valid operator
    if (t.token_type == PLUS || t.token_type == MINUS || t.token_type == MULT || t.token_type == DIV) {
        return;
    } else {
        syntax_error();
    }
}

void Parser::parse_output_stmt() {
    // Get the next token from the lexer and ensure it is an OUTPUT token
    Token t = lexer.GetToken();
    if (t.token_type != OUTPUT) {
        syntax_error();
    }

    // Get the next token and ensure it is an identifier
    t = lexer.GetToken();
    if (t.token_type != ID) {
        syntax_error();
    }

    // Get the next token and ensure it is a semicolon
    t = lexer.GetToken();
    if (t.token_type != SEMICOLON) {
        syntax_error();
    }
}

void Parser::parse_input_stmt() {
    // Get the next token and ensure it is an INPUT token
    Token t = lexer.GetToken();
    if (t.token_type != INPUT) {
        syntax_error();
    }

    // Get the next token and ensure it is an identifier
    t = lexer.GetToken();
    if (t.token_type != ID) {
        syntax_error();
    }

    // Get the next token and ensure it is a semicolon
    t = lexer.GetToken();
    if (t.token_type != SEMICOLON) {
        syntax_error();
    }
}

void Parser::parse_while_stmt() {
    // Expect a WHILE token
    expect(WHILE);
    
    // Parse the condition of the while loop
    parse_condition();
    
    // Parse the body of the while loop
    parse_body();
}

void Parser::parse_if_stmt() {
    // Expect an IF token
    expect(IF);
    
    // Parse the condition of the if statement
    parse_condition();
    
    // Parse the body of the if statement
    parse_body();
}

void Parser::parse_condition() {
    // Parse the first primary expression in the condition
    parse_primary();
    
    // Parse the relational operator in the condition
    parse_relop();
    
    // Parse the second primary expression in the condition
    parse_primary();
}

void Parser::parse_relop() {
    // Get the next token from the lexer
    Token t = lexer.GetToken();
    
    // Check if the token is a valid relational operator
    if (t.token_type == GREATER || t.token_type == LESS || t.token_type == NOTEQUAL) {
        return;
    } else {
        syntax_error();
    }
}

void Parser::parse_switch_stmt() {
    // Expect a SWITCH token
    expect(SWITCH);
    
    // Expect an identifier token
    expect(ID);
    
    // Expect a left brace token
    expect(LBRACE);
    
    // Parse the first case in the switch statement
    parse_case();

    // If the next token is a DEFAULT token, parse the default case
    if (lexer.peek(1).token_type == DEFAULT) {
        parse_default_case();
    }
    
    // Expect a right brace token
    expect(RBRACE);
}

void Parser::parse_for_stmt() {
    // Expect a FOR token
    expect(FOR);
    
    // Expect a left parenthesis token
    expect(LPAREN);
    
    // Parse the initialization statement of the for loop
    parse_assign_stmt();
    
    // Parse the condition of the for loop
    parse_condition();
    
    // Expect a semicolon token
    expect(SEMICOLON);
    
    // Parse the update statement of the for loop
    parse_assign_stmt();
    
    // Expect a right parenthesis token
    expect(RPAREN);
    
    // Parse the body of the for loop
    parse_body();
}

void Parser::parse_case_list() {
    // Parse an individual case in the switch statement
    parse_case();

    // If the next token is a CASE token, continue parsing the case list
    if (lexer.peek(1).token_type == CASE) {
        parse_case_list();
    }
}

void Parser::parse_case() {
    // Expect a CASE token
    expect(CASE);
    
    // Expect a number token
    expect(NUM);
    
    // Expect a colon token
    expect(COLON);
    
    // Parse the body of the case
    parse_body();
}

void Parser::parse_default_case() {
    // Expect a DEFAULT token
    expect(DEFAULT);
    
    // Expect a colon token
    expect(COLON);
    
    // Parse the body of the default case
    parse_body();
}

void Parser::parse_inputs() {
    // Parse a list of numbers
    parse_num_list();
}

void Parser::parse_num_list() {
    // Expect a number token
    expect(NUM);
    
    // If the next token is a number, continue parsing the number list
    if (lexer.peek(1).token_type == NUM) {
        parse_num_list();
    }
}