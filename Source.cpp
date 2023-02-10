#include <iostream>
#include <string>
#include <fstream>
using namespace std;
const int S_TABLE_SIZE = 10;
ifstream file;
int s_index = 0;
enum class TokenType {
	DIGIT, VARIABLE, SPACE, EQUAL_TO, NOT_EQUAL, ASSIGN_TO, COMPARISON, ADD_AND_ASSIGN,
	INCREMENT, ADD, SUB_AND_ASSIGN, DECREMENT, SUB, MOD,
	COMMENT, DIV_AND_ASSIGN, DIV, MUL_AND_ASSIGN, MUL,
	MOD_AND_ASSIGN, BITWISE, ADDRESS_OPERATOR, ERROR,
	GREATER_OR_EQUAL, GREATER, LESSER_OR_EQUAL, LESSER,
	COMMA, SEMICOLON, PARENTHESIS_OPEN, PARENTHESIS_CLOSE, COLON,
	SQUARE_OPEN, SQUARE_CLOSE, CURLY_OPEN, CURLY_CLOSE, IF, ELSE, DO, WHILE, BEGIN, END
};
struct Token
{
	TokenType type;
	string name;
	int ch;
};
Token Get_Token(TokenType _type, string _name, int _ch)
{
	Token tk;
	tk.type = _type;
	tk.name = _name;
	tk.ch = _ch;
	return tk;
}
struct SymbolTable {
	int ID;
	string name;
};
SymbolTable s_table[S_TABLE_SIZE];
int Search_Entry(string _name)
{
	for (int i = 0; i < S_TABLE_SIZE; i++)
	{
		if (s_table[i].name == _name)
		{
			return s_table[i].ID;
		}
	}
	return -1;
}
int Add_Entry(string _name) {
	int id = Search_Entry(_name);
	if (id == -1) {
		s_table[s_index].ID = s_index;
		s_table[s_index].name = _name;
		return s_index++;
	}
	return id;
}
//Globally
Token Tk;
bool bflag = false;
//---Functions---
//bool Assign();
//bool Exp();
bool Syntax_Check();
//bool Arthmetic();
bool Condition();
Token Lexical();
//Get token
Token Gettoken();
//Assignment Function
bool Assignment();
//If Statemnet Function
bool Stat();
//Begin & End Function
bool Begin_end();
//Tree Function
typedef enum { opk, constk, idk } nodekind;

struct Treenode
{
	nodekind kind;
	int value;
	char* name;
	TokenType opr;
	Treenode* leftchild;
	Treenode* rightchild;
};

Treenode* Exp();
Treenode* Arthmetic();
Treenode* Assign();

//Get Token
Token Gettoken()
{
	if (!bflag)
	{
		Tk = Lexical();
		return Tk;
	}
	else
	{
		bflag = false;
		return Tk;
	}
}

//If Praser
bool Stat() //IF(<EXP>){<ASSIGNMENT>} ELSE {<ASSIGNMENT>}
{
	Tk = Lexical();
	if (Tk.type == TokenType::IF) //IF
	{
		Tk = Lexical();
		if (Tk.type == TokenType::PARENTHESIS_OPEN)
		{
			if (Syntax_Check())
			{
				Tk = Lexical();
				if (Tk.type == TokenType::PARENTHESIS_CLOSE)
				{
					Tk = Lexical();
					if (Tk.type == TokenType::CURLY_OPEN)
					{
						if (Assignment())
						{
							if (Tk.type == TokenType::CURLY_CLOSE)
							{
								Tk = Lexical();
							}
							else
							{
								if (Tk.type == TokenType::ELSE) //ELSE
								{
									Tk = Lexical();
									if (Tk.type == TokenType::CURLY_OPEN)
									{
										if (Assignment())
										{
											Tk = Lexical();
											if (Tk.type == TokenType::CURLY_CLOSE)
											{
												return true;
											}
										}
									}
								}
								return true;
							}
						}
					}
				}
			}
		}
		return false;
	}
	else if (Tk.type == TokenType::DO)       //<DO><{EXPRESSION}><WHILE><SEMICOLON>
	{
		Tk = Lexical();
		if (Tk.type == TokenType::CURLY_OPEN)
		{
			Tk = Lexical();
			if (Syntax_Check())
			{
				Tk = Lexical();
				if (Tk.type == TokenType::CURLY_CLOSE)
				{
					Tk = Lexical();
					if (Tk.type == TokenType::WHILE)			//<WHILE><(EXPRESSION)><STATEMENT>
					{
						Tk = Lexical();
						if (Tk.type == TokenType::PARENTHESIS_OPEN)
						{
							if (Syntax_Check())
							{
								Tk = Lexical();
								if (Tk.type == TokenType::PARENTHESIS_CLOSE)
								{
									Tk = Lexical();
									if (Tk.type == TokenType::SEMICOLON)
									{
										return true;
									}
								}
							}
						}
					}
				}
			}
		}
		return false;
	}
}
//Praser
bool Assignment()						//<ASSIGN> -> ID <OPERATOR><EXP>;
{										//<ASSIGN> -> ARTHMETICINAL ARTHMETICAL NON - ARTHMETICINAL
		Tk = Lexical();
	if (Tk.type == TokenType::VARIABLE)
	{
		Tk = Lexical();
		if (Tk.type == TokenType::ASSIGN_TO)
		{
			if (Syntax_Check())
			{
				Tk = Lexical();
				if (Tk.type == TokenType::SEMICOLON)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool Condition()
{
	Tk = Lexical();
	if (Tk.type == TokenType::VARIABLE || Tk.type == TokenType::DIGIT)
	{
		Tk = Lexical();
		if (Tk.type == TokenType::COMPARISON)
		{
			Tk = Lexical();
			if (Tk.type == TokenType::VARIABLE || Tk.type == TokenType::DIGIT)
			{
				return true;
			}
		}
	}
	return false;
}

bool Begin_end()
{
	Tk = Lexical();
	//do {
		if (Tk.type == TokenType::BEGIN)
		{
			if (Stat())
			{
				Tk = Lexical();
				if (Tk.type == TokenType::END)
				{
					return true;
				}
			}
		}
	//} while (1);
}

Treenode* Assign() //DIGIT VARAIABLE 
{
	Treenode* Tr{};
	Tk = Lexical();
	if (Tk.type == TokenType::DIGIT)
	{
		Tr = new Treenode;
		Tr->kind = constk;
		Tr->value = stoi(Tk.name);
		Tr->name = NULL;
		Tr->leftchild = NULL;
		Tr->rightchild = NULL;
		return(Tr);
	}
	else if (Tk.type == TokenType::VARIABLE)
	{
		Tr = new Treenode;
		Tr->kind = idk;
		Tr->name = NULL;
		Tr->leftchild = NULL;
		Tr->rightchild = NULL;
		return(Tr);
	}
	else if (Tk.type == TokenType::PARENTHESIS_OPEN)
	{
		if (Tr=Exp())
		{
			Tk = Lexical();
			if (Tk.type == TokenType::PARENTHESIS_CLOSE)
			{
				return (Tr);
			}
		}
		return Tr;
	}
}

Treenode* Arthmetic() //MULTIPLY & DIVIDE
{
	Treenode* Tr, * Point = nullptr;
	int Flag = 0;
	do
	{
		Tr = Assign();
		if (Flag)
		{
			Point->rightchild = Tr;
			Flag = 0;
			Tr = Point;
		}
		Tk = Lexical();
		if(Tk.type == TokenType::MUL || Tk.type == TokenType::DIV || Tk.type == TokenType::MOD)
		{
			Point = new Treenode;
			Point->kind = opk;
			Point->opr = Tk.type;
			Point->leftchild = Tr;
			Flag = 1;
		}
	} while (Tk.type == TokenType::MUL || Tk.type == TokenType::DIV || Tk.type ==
		TokenType::MOD);
	return (Tr);
}

Treenode* Exp() //MINUS & PLUS
{
	Treenode* Tr, * Point = nullptr;
	int Flag = 0;
	do
	{
		Tr = Arthmetic();
		if (Flag)
		{
			Point->rightchild = Tr;
			Flag = 0;
			Tr = Point;
		}
		Tk = Lexical();
		if(Tk.type==TokenType::ADD || Tk.type == TokenType::SUB)
		{
			Point = new Treenode;
			Point->kind = opk;
			Point->opr = Tk.type;
			Point->leftchild = Tr;
			Flag = 1;
		}
	} while (Tk.type == TokenType::ADD || Tk.type == TokenType::SUB);
	return (Tr);
}

bool Syntax_Check()
{
	if (Exp())
	{
		return true;
	}
	else
	{
		return false;
	}
}

//Lexical
Token Lexical()
{
	char ch;
	while (file.get(ch)) {
		string buffer = "";
		switch (ch)
		{
		case ' ':
			return Get_Token(TokenType::SPACE, "EMAN_SPACE", ch);
			break;
		case '=':
			if (!file.get(ch))ch = NULL;
			switch (ch)
			{
			case '=':
				return Get_Token(TokenType::COMPARISON, "EMAN_EQUAL_TO", ch);
			default:
				file.putback(ch);
				return Get_Token(TokenType::ASSIGN_TO, "EMAN_ASSIGN_TO", ch);
			}
		case '+':
			if (!file.get(ch))ch = NULL;
			switch (ch)
			{
			case '=':
				return Get_Token(TokenType::ADD_AND_ASSIGN,
					"EMAN_ADD_AND_ASSIGN", ch);
			case '+':
				return Get_Token(TokenType::INCREMENT, "EMAN_INCREMENT", ch);
			default:
				file.putback(ch);
				return Get_Token(TokenType::ADD, "EMAN_ADD", ch);
			}
		case '-':
			if (!file.get(ch))ch = NULL;
			switch (ch)
			{
			case '=':
				return Get_Token(TokenType::SUB_AND_ASSIGN,
					"EMAN_SUB_AND_ASSIGN", ch);
			case '-':
				return Get_Token(TokenType::DECREMENT, "EMAN_DECREMENT", ch);
			default:
					file.putback(ch);
				return Get_Token(TokenType::SUB, "EMAN_SUB", ch);
			}
		case '/':
			if (!file.get(ch))ch = NULL;
			switch (ch)
			{
			case '/':
				do {
					if (!file.get(ch))ch = '\n';
				} while (ch != '\n');
				return Get_Token(TokenType::COMMENT, "EMAN_COMMENT", ch);
			case '=':
				return Get_Token(TokenType::DIV_AND_ASSIGN,
					"EMAN_DIV_AND_ASSIGN", ch);
			default:
				file.putback(ch);
				return Get_Token(TokenType::DIV, "EMAN_DIV", ch);
			}
			break;
		case '*':
			if (!file.get(ch))ch = NULL;
			switch (ch)
			{
			case '=':
				return Get_Token(TokenType::MUL_AND_ASSIGN,
					"EMAN_MUL_AND_ASSIGN", ch);
			default:
				file.putback(ch);
				return Get_Token(TokenType::MUL, "EMAN_MUL", ch);
			}
			break;
		case '%':
			if (!file.get(ch))ch = NULL;
			switch (ch)
			{
			case '=':
				return Get_Token(TokenType::MOD_AND_ASSIGN,
					"EMAN_MOD_AND_ASSIGN", ch);
			default:
				file.putback(ch);
				return Get_Token(TokenType::MOD, "EMAN_MOD", ch);
			}
			break;
		case '&':
			if (!file.get(ch))ch = NULL;
			switch (ch)
			{
			case '=':
				return Get_Token(TokenType::BITWISE, "EMAN_BITWISE", ch);
			default:
				file.putback(ch);
				return Get_Token(TokenType::ADDRESS_OPERATOR,
					"EMAN_ADDRESS_OPERATOR", ch);
			}
			break;
		case '>':
			if (!file.get(ch))ch = NULL;
				switch (ch)
				{
				case '=':
					return Get_Token(TokenType::COMPARISON, "EMAN_GREATER_OR_EQUAL",
						ch);
				default:
					file.putback(ch);
					return Get_Token(TokenType::COMPARISON, "EMAN_GREATER", ch);
				}
			break;
		case '<':
			if (!file.get(ch))ch = NULL;
			switch (ch)
			{
			case '=':
				return Get_Token(TokenType::COMPARISON, "EMAN_LESSER_OR_EQUAL",
					ch);
			default:
				file.putback(ch);
				return Get_Token(TokenType::COMPARISON, "EMAN_LESSER", ch);
			}
			break;
		case ',':
			return Get_Token(TokenType::COMMA, "EMAN_COMMA", ch); break;
		case ';':
			return Get_Token(TokenType::SEMICOLON, "EMAN_SEMICOLON", ch); break;
		case ':':
			return Get_Token(TokenType::COLON, "EMAN_COLON", ch); break;
		case '(':
			return Get_Token(TokenType::PARENTHESIS_OPEN, "EMAN_PARENTHESIS_OPEN",
				ch); break;
		case ')':
			return Get_Token(TokenType::PARENTHESIS_CLOSE,
				"EMAN_PARENTHESIS_CLOSE", ch); break;
		case '[':
			return Get_Token(TokenType::SQUARE_OPEN, "EMAN_SQUARE_OPEN", ch);
			break;
		case ']':
			return Get_Token(TokenType::SQUARE_CLOSE, "EMAN_SQUARE_CLOSE", ch);
			break;
		case '{':
			return Get_Token(TokenType::CURLY_OPEN, "EMAN_CURLY_OPEN", ch); break;
		case '}':
			return Get_Token(TokenType::CURLY_CLOSE, "EMAN_CURLY_CLOSE", ch);
			break;
		default:
			if (isdigit(ch)) {
				do {
					buffer += char(ch);
					if (!file.get(ch))ch = NULL;
				} while (isdigit(ch));
				file.putback(ch);
				return Get_Token(TokenType::DIGIT, "EMAN_DIGIT", ch);
			}
			else if (isalpha(ch))
			{
				do {
					buffer += ch;
						if (!file.get(ch))ch = NULL;
				} while (isalnum(ch) || ch == '_');
				file.putback(ch);
				if (buffer == "if")
				{
					//file.putback(ch);
					return Get_Token(TokenType::IF, "EMAN_IF", ch);
				}
				else if (buffer == "else")
				{
					//file.putback(ch);
					return Get_Token(TokenType::ELSE, "EMAN_ELSE", ch);
				}
				else if (buffer == "do")
				{
					//file.putback(ch);
					return Get_Token(TokenType::DO, "EMAN_DO", ch);
				}
				else if (buffer == "while")
				{
					//file.putback(ch);
					return Get_Token(TokenType::WHILE, "EMAN_WHILE", ch);
				}
				else if (buffer == "BEGIN")
				{
					//file.putback(ch);
					return Get_Token(TokenType::BEGIN, "EMAN_BEGIN", ch);
				}
				else if (buffer == "END")
				{
					//file.putback(ch);
					return Get_Token(TokenType::END, "EMAN_END", ch);
				}
				Add_Entry(buffer);
				return Get_Token(TokenType::VARIABLE, "EMAN_VARIABLE", ch);
			}
			break;
		}
	}
	return Get_Token(TokenType::ERROR, "ERROR", 0);
}

int main()
{
	file.open("file.txt");
	if (!file)
	{
		cout << "ERROR: File does not exist." << endl;
		exit(0);
	}
	/*Token token;
	cout << "\n\tToken" << endl;
	cout << "\t==============" << endl;
	while (!file.eof())
	{
	token = Lexical();
	if (token.type == TokenType::ERROR)
	break;
	cout << "\t" << token.name << endl;
	}*/
	if (file.is_open())
	{
		if (Begin_end())
		{
			cout << "\n\t===============" << endl;
			cout << "\tSyntax Checking\n\tCorrect!" << endl;
		}
		else
		{
			cout << "\n\t===============" << endl;
			cout << "\tSyntax Checking\n\tIncorrect!" << endl;
		}
	}
	file.close();
	return 0;
}
 
