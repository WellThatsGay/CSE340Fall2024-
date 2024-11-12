/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <algorithm>
#include "lexer.h"

using namespace std;

unordered_map<string, vector<vector<string>>> rules;  // Stores grammar rules
unordered_set<string> nonTerminals, terminals;        // Sets of non-terminals and terminals
string startSymbol;                                   // Start symbol of the grammar
unordered_map<string, unordered_set<string>> firstSets, followSets;  // Maps for FIRST and FOLLOW sets
unordered_set<string> nullable;                       // Set of nullable non-terminals

void ReadGrammar();
void Task1();
void Task2();
void Task3();
void Task4();
void Task5();
void Task6();

// Read the grammar from input
void ReadGrammar() {
    LexicalAnalyzer lexer;
    Token token = lexer.GetToken();
    vector<string> rhs;
    string lhs;

    while (token.token_type != HASH) {
        if (token.token_type == ID) {
            lhs = token.lexeme;
            nonTerminals.insert(lhs);
            token = lexer.GetToken();
            if (token.token_type == ARROW) {
                rhs.clear();
                token = lexer.GetToken();
                while (token.token_type != STAR) {
                    rhs.push_back(token.lexeme);
                    if (token.token_type == ID && nonTerminals.find(token.lexeme) == nonTerminals.end()) {
                        terminals.insert(token.lexeme);
                    }
                    token = lexer.GetToken();
                }
                rules[lhs].push_back(rhs);
            }
        }
        token = lexer.GetToken();
    }

    if (!rules.empty()) {
        startSymbol = rules.begin()->first;
    }
}

// Task 1: Print terminals and non-terminals
void Task1() {
    for (const auto& term : terminals) {
        cout << term << " ";
    }
    cout << endl;
    for (const auto& nonTerm : nonTerminals) {
        cout << nonTerm << " ";
    }
    cout << endl;
}

// Task 2: Calculate nullable non-terminals
void Task2() {
    bool updated;
    do {
        updated = false;
        for (const auto& rule : rules) {
            if (nullable.count(rule.first)) continue;
            for (const auto& rhs : rule.second) {
                bool isNullable = true;
                for (const auto& sym : rhs) {
                    if (!nullable.count(sym)) {
                        isNullable = false;
                        break;
                    }
                }
                if (isNullable) {
                    nullable.insert(rule.first);
                    updated = true;
                    break;
                }
            }
        }
    } while (updated);

    cout << "Nullable = { ";
    for (auto it = nullable.begin(); it != nullable.end(); ++it) {
        if (it != nullable.begin()) cout << ", ";
        cout << *it;
    }
    cout << " }" << endl;
}

// Task 3: Calculate FIRST sets
void Task3() {
    auto computeFirst = [&](const string& symbol) -> unordered_set<string> {
        if (terminals.count(symbol)) return { symbol };
        if (firstSets.count(symbol)) return firstSets[symbol];

        unordered_set<string> first;
        for (const auto& rhs : rules[symbol]) {
            for (const auto& sym : rhs) {
                auto firstSym = computeFirst(sym);
                first.insert(firstSym.begin(), firstSym.end());
                if (!nullable.count(sym)) break;
            }
        }
        return firstSets[symbol] = first;
        };

    for (const auto& nt : nonTerminals) {
        firstSets[nt] = computeFirst(nt);
        cout << "FIRST(" << nt << ") = { ";
        for (auto it = firstSets[nt].begin(); it != firstSets[nt].end(); ++it) {
            if (it != firstSets[nt].begin()) cout << ", ";
            cout << *it;
        }
        cout << " }" << endl;
    }
}

// Task 4: Calculate FOLLOW sets
void Task4() {
    followSets[startSymbol].insert("$");

    bool updated;
    do {
        updated = false;
        for (const auto& rule : rules) {
            for (const auto& rhs : rule.second) {
                for (size_t i = 0; i < rhs.size(); ++i) {
                    if (nonTerminals.count(rhs[i])) {
                        unordered_set<string> follow;
                        if (i + 1 < rhs.size()) {
                            auto firstNext = firstSets[rhs[i + 1]];
                            follow.insert(firstNext.begin(), firstNext.end());
                            follow.erase("");
                        }
                        if (i + 1 == rhs.size() || nullable.count(rhs[i + 1])) {
                            follow.insert(followSets[rule.first].begin(), followSets[rule.first].end());
                        }
                        size_t prevSize = followSets[rhs[i]].size();
                        followSets[rhs[i]].insert(follow.begin(), follow.end());
                        updated = updated || (followSets[rhs[i]].size() > prevSize);
                    }
                }
            }
        }
    } while (updated);

    for (const auto& nt : nonTerminals) {
        cout << "FOLLOW(" << nt << ") = { ";
        for (auto it = followSets[nt].begin(); it != followSets[nt].end(); ++it) {
            if (it != followSets[nt].begin()) cout << ", ";
            cout << *it;
        }
        cout << " }" << endl;
    }
}

// Task 5: Left-factor the grammar
void Task5() {
    for (const auto& rule : rules) {
        vector<vector<string>> newRules;
        unordered_map<string, vector<vector<string>>> prefixGroups;

        for (const auto& rhs : rule.second) {
            if (!rhs.empty()) {
                prefixGroups[rhs[0]].push_back(rhs);
            }
        }

        for (auto& [prefix, group] : prefixGroups) {
            if (group.size() > 1) {
                string newNonTerm = rule.first + "_prime";
                vector<string> newRhs = { prefix, newNonTerm };
                newRules.push_back(newRhs);
                for (auto& suffix : group) {
                    suffix.erase(suffix.begin());
                    rules[newNonTerm].push_back(suffix.empty() ? vector<string>{""} : suffix);
                }
            }
            else {
                newRules.push_back(group[0]);
            }
        }
        rules[rule.first] = newRules;
    }

    for (const auto& rule : rules) {
        for (const auto& rhs : rule.second) {
            cout << rule.first << " -> ";
            for (const auto& sym : rhs) {
                cout << sym << " ";
            }
            cout << "*" << endl;
        }
    }
}

// Task 6: Eliminate left recursion
void Task6() {
    for (const auto& rule : rules) {
        vector<vector<string>> nonRecursive;
        vector<vector<string>> recursive;

        for (const auto& rhs : rule.second) {
            if (!rhs.empty() && rhs[0] == rule.first) {
                vector<string> newRhs(rhs.begin() + 1, rhs.end());
                newRhs.push_back(rule.first + "_prime");
                recursive.push_back(newRhs);
            }
            else {
                vector<string> newRhs = rhs;
                newRhs.push_back(rule.first + "_prime");
                nonRecursive.push_back(newRhs);
            }
        }

        if (!recursive.empty()) {
            rules[rule.first] = nonRecursive;
            string newNonTerm = rule.first + "_prime";
            for (auto& rhs : recursive) {
                rules[newNonTerm].push_back(rhs);
            }
            rules[newNonTerm].push_back({ "" });
        }
    }

    for (const auto& rule : rules) {
        for (const auto& rhs : rule.second) {
            cout << rule.first << " -> ";
            for (const auto& sym : rhs) {
                cout << sym << " ";
            }
            cout << "*" << endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Error: missing argument\n";
        return 1;
    }

    int task = atoi(argv[1]);
    ReadGrammar();

    switch (task) {
    case 1: Task1(); break;
    case 2: Task2(); break;
    case 3: Task3(); break;
    case 4: Task4(); break;
    case 5: Task5(); break;
    case 6: Task6(); break;
    default:
        cout << "Error: unrecognized task number " << task << "\n";
        break;
    }
    return 0;
}
