#include "execute.h"
#include "lexer.h"
#include "parser.h"
#include <vector>
#include <algorithm>

/*Christopher Harris*/
/*Project 3*/
/*CSE 340*/

LexicalAnalyzer lexer;
using namespace std;

vector<string> table;

// Declaration of parsing functions
struct InstructionNode * parse_body();

int location(string str) {
    for (int i = 0; i < table.size(); i++) {
        if (table[i] == str)
            return i;
    }
    return -1; // Add a return value if the string wasn't found
}


void parse_id_list() {
    Token token = lexer.GetToken();
    while (token.token_type != SEMICOLON) {
        table.push_back(token.lexeme);  
        mem[location(token.lexeme)] = 0;      
        token = lexer.GetToken();             
    }
}

struct InstructionNode * parse_assign_stmt() {
    // Create a new instruction node for the assignment statement
    struct InstructionNode * parseAssignStatement = new InstructionNode;
    parseAssignStatement->next = NULL;

    // Get the next token from the lexer
    Token t = lexer.GetToken();

    // Check if the token is an identifier
    if (t.token_type == ID) {
        parseAssignStatement->type = ASSIGN;
        parseAssignStatement->assign_inst.left_hand_side_index = location(t.lexeme);
    }

    // Get the next token and ensure it is an equal sign
    t = lexer.GetToken();  
    if (t.token_type == EQUAL) {
        t = lexer.GetToken();  
        
        // Determine the type of the right-hand side operand
        switch (t.token_type) {
            case ID:
                parseAssignStatement->assign_inst.opernd1_index = location(t.lexeme);
                break;
            case NUM:
                // Add numeric value to the table if it's not already present
                if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                    table.push_back(t.lexeme);
                    mem[location(t.lexeme)] = stoi(t.lexeme);
                }
                parseAssignStatement->assign_inst.opernd1_index = location(t.lexeme);
                break;
            case END_OF_FILE:
                // Handle the END_OF_FILE case appropriately
                delete parseAssignStatement;  // Clean up before returning
                return nullptr;  // Return a null pointer to indicate an error
                break;
            default:
                // Optionally handle any other unanticipated token types
                break;
        }

        // Get the next token and check if it is a semicolon
        t = lexer.GetToken();  
        if (t.token_type == SEMICOLON) {
            parseAssignStatement->assign_inst.op = OPERATOR_NONE;
            parseAssignStatement->assign_inst.opernd2_index = 0;
        } else {
            // Determine the operator and get the second operand
            if (t.token_type == PLUS)
                parseAssignStatement->assign_inst.op = OPERATOR_PLUS;
            else if (t.token_type == MINUS)
                parseAssignStatement->assign_inst.op = OPERATOR_MINUS;
            else if (t.token_type == MULT)
                parseAssignStatement->assign_inst.op = OPERATOR_MULT;
            else if (t.token_type == DIV)
                parseAssignStatement->assign_inst.op = OPERATOR_DIV;

            // Get the next token to identify the second operand
            t = lexer.GetToken();  
            switch (t.token_type) {
                case ID:
                    parseAssignStatement->assign_inst.opernd2_index = location(t.lexeme);
                    break;
                case NUM:
                    // Add numeric value to the table if it's not already present
                    if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                        table.push_back(t.lexeme);
                        mem[location(t.lexeme)] = stoi(t.lexeme);
                    }
                    parseAssignStatement->assign_inst.opernd2_index = location(t.lexeme);
                    break;
                case END_OF_FILE:
                    // Handle the END_OF_FILE case appropriately
                    delete parseAssignStatement;  // Clean up before returning
                    return nullptr;  // Return a null pointer to indicate an error
                    break;
            }
            t = lexer.GetToken();  
        }
    }

    // Return the constructed instruction node for the assignment statement
    return parseAssignStatement;
}

struct InstructionNode * parse_while_stmt() {
    // Create a new instruction node for the while statement
    struct InstructionNode * whileStatement = new InstructionNode;
    whileStatement->next = NULL;

    // Expecting a WHILE token
    Token t = lexer.GetToken();  

    if (t.token_type == WHILE) {
        whileStatement->type = CJMP;

        // Get the condition variable or value
        t = lexer.GetToken();  
        switch (t.token_type) {
            case ID:
                whileStatement->cjmp_inst.opernd1_index = location(t.lexeme);
                break;
            case NUM:
                // If numeric value is not already present, add it to the table and initialize in memory
                if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                    table.push_back(t.lexeme);
                    mem[location(t.lexeme)] = stoi(t.lexeme);
                }
                whileStatement->cjmp_inst.opernd1_index = location(t.lexeme);
                break;
        }

        // Get the comparison operator
        t = lexer.GetToken();  
        if (t.token_type == GREATER)
            whileStatement->cjmp_inst.condition_op = CONDITION_GREATER;
        else if (t.token_type == LESS)
            whileStatement->cjmp_inst.condition_op = CONDITION_LESS;
        else if (t.token_type == NOTEQUAL)
            whileStatement->cjmp_inst.condition_op = CONDITION_NOTEQUAL;

        // Get the operand for comparison
        t = lexer.GetToken();  
        switch (t.token_type) {
            case ID:
                whileStatement->cjmp_inst.opernd2_index = location(t.lexeme);
                break;
            case NUM:
                if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                    table.push_back(t.lexeme);
                    mem[location(t.lexeme)] = stoi(t.lexeme);
                }
                whileStatement->cjmp_inst.opernd2_index = location(t.lexeme);
                break;
        }

        // Parse the body of the while loop
        whileStatement->next = parse_body();  

        // Create and link a jump node to implement the loop back to the start of the while
        struct InstructionNode* jumpNode = new InstructionNode;
        jumpNode->type = JMP;
        jumpNode->jmp_inst.target = whileStatement;

        // Link the jump node to the end of the while body
        struct InstructionNode* temp = whileStatement->next;
        while (temp->next != NULL) {
            temp = temp->next;  
        }
        temp->next = jumpNode;  

        // Create a NOOP node to mark the end of the while loop
        struct InstructionNode * noop = new InstructionNode;  
        noop->type = NOOP;
        noop->next = NULL;
        jumpNode->next = noop;  // Link jump node to noop

        // Set the target of the conditional jump to the NOOP node
        whileStatement->cjmp_inst.target = noop;  
    }

    // Return the constructed instruction node for the while loop
    return whileStatement;
}

struct InstructionNode * parse_if_stmt() {
    // Create a new instruction node for the if statement
    struct InstructionNode * ifStatement = new InstructionNode;
    ifStatement->next = NULL;

    // Expecting an IF token
    Token t = lexer.GetToken();

    if (t.token_type == IF) {
        ifStatement->type = CJMP;

        // Get the condition variable or value
        t = lexer.GetToken();
        switch (t.token_type) {
            case ID:
                ifStatement->cjmp_inst.opernd1_index = location(t.lexeme);
                break;
            case NUM:
                // If numeric value is not already present, add it to the table and initialize in memory
                if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                    table.push_back(t.lexeme);
                    mem[location(t.lexeme)] = stoi(t.lexeme);
                }
                ifStatement->cjmp_inst.opernd1_index = location(t.lexeme);
                break;
        }

        // Get the comparison operator
        t = lexer.GetToken();
        if (t.token_type == GREATER)
            ifStatement->cjmp_inst.condition_op = CONDITION_GREATER;
        else if (t.token_type == LESS)
            ifStatement->cjmp_inst.condition_op = CONDITION_LESS;
        else if (t.token_type == NOTEQUAL)
            ifStatement->cjmp_inst.condition_op = CONDITION_NOTEQUAL;

        // Get the operand for comparison
        t = lexer.GetToken();
        switch (t.token_type) {
            case ID:
                ifStatement->cjmp_inst.opernd2_index = location(t.lexeme);
                break;
            case NUM:
                if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                    table.push_back(t.lexeme);
                    mem[location(t.lexeme)] = stoi(t.lexeme);
                }
                ifStatement->cjmp_inst.opernd2_index = location(t.lexeme);
                break;
        }

        // Parse the body of the if statement
        ifStatement->next = parse_body();

        // Create a NOOP node to mark the end of the if statement
        struct InstructionNode * noop = new InstructionNode;
        noop->type = NOOP;
        noop->next = NULL;

        // Link the NOOP node to the end of the if body
        struct InstructionNode * temp = ifStatement->next;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = noop;

        // Set the target of the conditional jump to the NOOP node
        ifStatement->cjmp_inst.target = noop;
    }

    // Return the constructed instruction node for the if statement
    return ifStatement;
}

struct InstructionNode * parse_case(InstructionNode * jump, int temp_operand) {
    // Creates a new instruction node for the case statement
    struct InstructionNode * instParseCase = new InstructionNode;
    instParseCase->type = CJMP;
    instParseCase->cjmp_inst.opernd1_index = temp_operand;
    instParseCase->cjmp_inst.condition_op = CONDITION_NOTEQUAL;  

    // Gets the next token from the lexer
    Token t = lexer.GetToken();  
    switch (t.token_type) {
        case CASE:
            t = lexer.GetToken();  
            if (t.token_type == ID) {
                instParseCase->cjmp_inst.opernd2_index = location(t.lexeme);
            }
            else if (t.token_type == NUM) {
                // Add numeric value to the table if it's not already present
                if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                    table.push_back(t.lexeme);
                    mem[location(t.lexeme)] = stoi(t.lexeme);
                }
                instParseCase->cjmp_inst.opernd2_index = location(t.lexeme);
            }
            break;

        case END_OF_FILE:
            // Handles the END_OF_FILE case appropriately
            return nullptr;  // Returns a null pointer to indicate an error
            break;

        case DEFAULT:
            printf("Default case for testing\n");
            break;
    }

    // Gets the next token from the lexer
    t = lexer.GetToken();  

    // Parses the body of the case statement
    instParseCase->cjmp_inst.target = parse_body();

    // Links the jump node to the end of the case statement
    struct InstructionNode * temp = instParseCase->cjmp_inst.target;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = jump;  

    // Checks if there are more case statements to parse
    t = lexer.peek(1);
    if (t.token_type == DEFAULT || t.token_type == CASE) {
        struct InstructionNode * i = parse_case(jump, temp_operand);
        instParseCase->next = i;  
    }

    // Return the constructed instruction node for the case statement
    return instParseCase;
}

struct InstructionNode * parse_switch_stmt() {
    // Initialize instruction nodes for the switch statement
    struct InstructionNode * switchStatement = nullptr;  // Initialized to nullptr
    struct InstructionNode * jump = new InstructionNode;
    struct InstructionNode * noop = new InstructionNode;

    // Sets up the NOOP node
    noop->type = NOOP;
    noop->next = NULL;

    // Sets up the jump node to target the NOOP node
    jump->type = JMP;
    jump->jmp_inst.target = noop;

    // Initializes the operand index with a default invalid index
    int temp_operand = -1;

    // Gets the next token from the lexer
    Token t = lexer.GetToken();
    if (t.token_type == SWITCH) {
        t = lexer.GetToken();
        if (t.token_type == ID) {
            temp_operand = location(t.lexeme);
        } else if (t.token_type == NUM) {
            // Adds numeric value to the table if it's not already present
            if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                table.push_back(t.lexeme);
                mem[location(t.lexeme)] = stoi(t.lexeme);
            }
            temp_operand = location(t.lexeme);
        } else {
            // Handles syntax error or unexpected token type
        }

        // Gets the next token from the lexer
        t = lexer.GetToken();

        // Parses the case statements
        switchStatement = parse_case(jump, temp_operand);

        // Ensures switchStatement is not nullptr before linking the NOOP node
        if (switchStatement != nullptr) {
            struct InstructionNode * temp = switchStatement;
            
            while (temp != NULL && temp->next != NULL) {
                temp = temp->next;
            }
            if (temp != NULL) {
                temp->next = noop;
            } else {
                // Handles the case where temp is nullptr, if necessary
                printf("This is a test if temp is nullptr\n");
            }
        } else {
            // Handle the case where switchStatement is nullptr, if necessary
            printf("This is a test if the switchStatement is nullptr\n");
        }

        // Get the next token from the lexer
        t = lexer.GetToken(); 

        // Return the constructed instruction node for the switch statement
        return switchStatement;
    }

    // Return nullptr if the SWITCH token is not found
    return nullptr;
}

struct InstructionNode * parse_for_stmt() {
    // Initialize instruction nodes for the for statement
    struct InstructionNode * forStatement = new InstructionNode; 
    struct InstructionNode * noop = new InstructionNode;  
    struct InstructionNode * jump = new InstructionNode;  
    
    // Set up the instruction types
    forStatement->type = CJMP;
    noop->type = NOOP;
    jump->type = JMP;
    
    // Set up the targets for the conditional jump and jump nodes
    forStatement->cjmp_inst.target = noop;  
    jump->jmp_inst.target = forStatement;  
    jump->next = noop;  
    noop->next = NULL;

    // Get the FOR token and the next token
    Token t = lexer.GetToken();  
    t = lexer.GetToken();  

    // Parse the initialization statement
    struct InstructionNode * instructionOne = parse_assign_stmt();  
    instructionOne->next = forStatement;  

    // Get the first operand for the condition
    t = lexer.GetToken();  
    if (t.token_type == ID)
        forStatement->cjmp_inst.opernd1_index = location(t.lexeme);  
    else if (t.token_type == NUM) {
        if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
            table.push_back(t.lexeme);
            mem[location(t.lexeme)] = stoi(t.lexeme);
        }
        forStatement->cjmp_inst.opernd1_index = location(t.lexeme); 
    }

    // Get the condition operator
    t = lexer.GetToken();  
    switch (t.token_type) {
        case GREATER:
            forStatement->cjmp_inst.condition_op = CONDITION_GREATER;
            break;
        case LESS:
            forStatement->cjmp_inst.condition_op = CONDITION_LESS;
            break;
        case NOTEQUAL:
            forStatement->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
            break;
    }

    // Get the second operand for the condition
    t = lexer.GetToken();  
    if (t.token_type == ID)
        forStatement->cjmp_inst.opernd2_index = location(t.lexeme);
    else if (t.token_type == NUM) {
        if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
            table.push_back(t.lexeme);
            mem[location(t.lexeme)] = stoi(t.lexeme);
        }
        forStatement->cjmp_inst.opernd2_index = location(t.lexeme);
    }
    
    // Get the next token
    t = lexer.GetToken();  

    // Parse the update statement
    struct InstructionNode * instructionTwo = parse_assign_stmt();  
    t = lexer.GetToken();  

    // Parse the body of the for loop
    struct InstructionNode * i_list = parse_body();  
    forStatement->next = i_list;  

    // Link the update statement to the end of the for loop body
    struct InstructionNode * temp = i_list;
    while (temp->next != NULL) {
        temp = temp->next;  
    }
    temp->next = instructionTwo;  
    
    // Link the update statement to the jump node
    instructionTwo->next = jump;  
    
    // Return the constructed instruction node for the initialization statement
    return instructionOne;  
}


struct InstructionNode * parse_output_stmt() {
    // Create a new instruction node for the output statement
    struct InstructionNode * outputStatement = new InstructionNode;
    outputStatement->next = NULL;

    // Get the OUTPUT token
    Token t = lexer.GetToken();
    outputStatement->type = OUT;

    // Get the identifier token
    t = lexer.GetToken();
    outputStatement->input_inst.var_index = location(t.lexeme);

    // Get the semicolon token
    t = lexer.GetToken();

    // Return the constructed instruction node for the output statement
    return outputStatement;
} 

struct InstructionNode * parse_input_stmt() {
    // Create a new instruction node for the input statement
    struct InstructionNode * inputStatement = new InstructionNode;
    inputStatement->next = NULL;

    // Get the INPUT token
    Token t = lexer.GetToken();
    inputStatement->type = IN;

    // Get the identifier token
    t = lexer.GetToken();
    inputStatement->input_inst.var_index = location(t.lexeme);

    // Get the semicolon token
    t = lexer.GetToken();

    // Return the constructed instruction node for the input statement
    return inputStatement;
}

struct InstructionNode * parse_stmt() {
    struct InstructionNode * instructionOne;

    // Peek at the next token to determine the type of statement
    Token t = lexer.peek(1);

    // Use if-else statements to handle different types of statements
    if (t.token_type == ID) {
        instructionOne = parse_assign_stmt();
    } else if (t.token_type == WHILE) {
        instructionOne = parse_while_stmt();
    } else if (t.token_type == IF) {
        instructionOne = parse_if_stmt();
    } else if (t.token_type == SWITCH) {
        instructionOne = parse_switch_stmt();
    } else if (t.token_type == FOR) {
        instructionOne = parse_for_stmt();
    } else if (t.token_type == OUTPUT) {
        instructionOne = parse_output_stmt();
    } else if (t.token_type == INPUT) {
        instructionOne = parse_input_stmt();
    } else {
        // syntax_error();
    }

    // Return the constructed instruction node for the statement
    return instructionOne;
}

struct InstructionNode * parse_stmt_list() {
    // These nodes represent individual statements or blocks of statements.
    struct InstructionNode * instructionOne;  // First statement in the list.
    struct InstructionNode * instructionTwo;  // Second statement in the list.

    // Peek the next token.
    Token t = lexer.peek(1);
    TokenType t_type = t.token_type;

    // Parse the first statement.
    instructionOne = parse_stmt();

    t = lexer.peek(1);

    // Check if the next token indicates another statement; continue parsing if true.
    if (t.token_type == ID || t.token_type == WHILE || t.token_type == IF ||
        t.token_type == SWITCH || t.token_type == FOR || t.token_type == OUTPUT || t.token_type == INPUT) {
        instructionTwo = parse_stmt_list();  // Recursively parse the following statements.

        // Special handling for control flow structures that may need to link back to a previous statement.
        if (instructionOne->type == CJMP || t_type == FOR) {
            struct InstructionNode * temp = instructionOne->next;  
            while (temp->next != NULL) {
    
                temp = temp->next;
            }
            temp->next = instructionTwo;  
        } else {
           
            instructionOne->next = instructionTwo;
        }
    }

    // Return the first statement in the list.
    return instructionOne;
}


struct InstructionNode * parse_body() {
    struct InstructionNode * instructionParseBody = nullptr;

    // Get the next token from the lexer
    Token t = lexer.GetToken();

    // Check if the token is a left brace
    if (t.token_type == LBRACE) {
        // Parse the statement list within the body
        instructionParseBody = parse_stmt_list();
    }

    // Get the next token from the lexer (expected to be a right brace)
    t = lexer.GetToken();
    
    // Return the constructed instruction node for the body
    return instructionParseBody;
}

void parse_inputs() {
    // Get the next token from the lexer
    Token t = lexer.GetToken();

    // Convert the token lexeme to an integer and add it to the inputs vector
    inputs.push_back(stoi(t.lexeme));

    // Peek at the next token to check if there are more inputs
    t = lexer.peek(1); 

    // If the next token is a number, continue parsing inputs
    if (t.token_type == NUM) {
        parse_inputs();
    }
}

struct InstructionNode * parse_Generate_Intermediate_Representation() {
    
    parse_id_list(); 

    // Parse the main body of the program.
    struct InstructionNode * temp = parse_body();

    // Parse inputs if any.
    parse_inputs();

    return temp;
}