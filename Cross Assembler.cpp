# include <iostream>
# include <fstream>
# include <string>
# include <vector>
# include <iomanip>
using namespace std;

struct labelAndAddress {
	string label;
	int address;
}; // labelAndAddress

struct line_finalOutput {
	string instruction_forwardReference;
	string operand1_forwardReference;
	string operand2_forwardReference;
	int pc_forwardReference;
	
	int pc;
	vector<string> machineCode;
	int machineCodeLength;
	string line_head; // line_head + line_tail is same of input
	string line_tail;
}; // labelAndAddress

class OneTable {
	public :
		vector<labelAndAddress> tokens;

	public :
		OneTable() {
		  tokens.clear();
		  labelAndAddress temp;
		  temp.label = "", temp.address = -1;
		  tokens.push_back(temp);
		} // constructor
}; // class OneTable

class File {
	public :
	  ifstream inf;
	  ofstream outf;
	  string inputFileName, outputFileName;
		vector<OneTable> allTable;

	public :
		File() {
			allTable.clear();

		  for ( int a = 0 ; a <= 4 ; a++ ) { // no.0 is useless
				OneTable table;
				allTable.push_back(table);
			} // for(a)
			
			for ( int a = 5 ; a <= 7 ; a++ ) {
				OneTable table;
				
				for ( int b = 1 ; b <= 99 ; b++ ) { // table.tokens.size() == 1, need to +99
				  labelAndAddress temp;
		      temp.label = "", temp.address = -1;
		      table.tokens.push_back(temp);
				} // for (b)
				
			  allTable.push_back(table);
			} // for(a)
		} // File()

		void readTable(string tableName) {
			int no = tableName[5] - '0';
			string token;
			inf.open(tableName.c_str());
			
			while ( getline(inf, token) ) {
				labelAndAddress temp;
		    temp.label = token, temp.address = -1;
		    allTable[no].tokens.push_back(temp);
			} // while()
			  
			inf.close();
		} // readTable()
		
		void OpenInputFile() {
      // test
			// inputFileName = "2.txt";
			// inf.open(inputFileName.c_str()); // try to open file

			do { // is the file exist ?
        cout << "Key in the input file name : ";
        cin >> inputFileName;
        inf.open(inputFileName.c_str()); // try to open file
        
        if ( !inf )
				  cout << "\n### File does not exist! ###\n\n";
				  
      } while (!inf); // while (!inf)
		} // OpenInputFile()

}; // class File

class Token{
	public :
 		int pc;
		bool isString, isStartingAddressSet, addTo4Digit_yes, addTo4Digit_no, isForwardReference;
		vector<line_finalOutput> finalOutput;
		line_finalOutput temp;

	public :
	  Token(File* file) {
			int tableNo;
			bool isTokenCutDone, isInstruction, getFirstToken;
			string line, token = "", instruction = "", operand1 = "", operand2 = "";
			tableNo = pc = 0;
			addTo4Digit_yes = true;
			isString = isStartingAddressSet = isTokenCutDone = isInstruction = addTo4Digit_no = isForwardReference = getFirstToken = false;
			finalOutput.clear();

			while ( getline(file->inf, line) ) {
				if ( line != "" ) {
					clear_line_finalOutput(temp);
					temp.pc = pc;
					
				  for ( int a = 0 ; a < line.size() ; a++ ) {
					  if      ( token == "" && line[a] != ' ' && line[a] != '\t' ) token += line[a];  // find the first char of token
					  else if ( token == ";" && !isString )                        break;             // comment, becareful ';'
					  else if ( isTokensSame(token, line[a]) )                     token += line[a];  // same token
					  else if ( token != "" && !isString && ( line[a] == ' ' || line[a] == '\t' ) ) { // bump into white space ( ' ' || '\t' ), cut token
              tableNo = findTokensHouse_whichTable(token, file);                            // white space in String is ok, don't cut it
              isTokenCutDone = true;
					  } // else if()
					  else if ( token != "" && !isTokensSame(token, line[a]) ) {                      // different token bumped into, so cut token
						  tableNo = findTokensHouse_whichTable(token, file);
						  isTokenCutDone = true;
						  a--;
					  } // else if()
						//----------------------------Token Cut Done--------------------------------------------
					  if ( isTokenCutDone ) {
							if ( !getFirstToken ) { // use to align
							  if ( token == "CODE" || tableNo == 5 ) {
									temp.line_head = token;
									
									int skipWhiteSpace = a;
									for ( int b = a ; b < line.size() ; b++ ) {
										if ( line[b] == ' ' || line[b] == '\t' ) skipWhiteSpace++;
										else break;
									} // for(b)
									temp.line_tail = line.substr(skipWhiteSpace, line.size() - skipWhiteSpace);
								} // if()
								else {
									temp.line_head = "";
									temp.line_tail = line.substr(a - token.size(), line.size() - a + token.size());
								} // else
								
								getFirstToken = true;
							} // if ( !getFirstToken )


						  if ( !isInstruction ) {
						    if ( tableNo == 1 ) {
								  instruction = token;
							    isInstruction = true;
                } // if(tableNo == 1)
                
                else if ( tableNo == 5 ) {  // set the symbol's address
								  for ( int a = 0 ; a <= 99 ; a++ ) {
									  if ( token == file->allTable[5].tokens[a].label ) { // we find it
										  if ( file->allTable[5].tokens[a].address == -1 ) file->allTable[5].tokens[a].address = pc; // if we didn't set it before, we set it now
										  break;
									  } // if()
								  } // for (a)
							  } // else if (tableNo == 5)
							  
						    else if ( tableNo == 6 || tableNo == 7 ) StringOrDigitTransToMachineCode_newPC(token, file);
					    } // if ( !isInstruction )

					    else { // this token maybe is Instruction's operand
							  if ( tableNo == 3 || tableNo == 5 || tableNo == 6 ) { // only this kind of token can be a operand
								  if ( operand1 == "" ) operand1 = token;
								  else                  operand2 = token;
							  } // if()
					    } // else ( isInstruction )

					    token = "";
					    isTokenCutDone = false;
					  } // if ( isTokenCutDone )

		      } // for(a), we deal all of the token on this line, except the last one

		      if ( token != "" ) {   // so we need to deal with the last one token
					  tableNo = findTokensHouse_whichTable(token, file);

					  if ( tableNo == 1 ) {
						  isInstruction = true;
              instruction = token;
            } // if()
					  else if ( isInstruction && ( tableNo == 3 || tableNo == 5 || tableNo == 6 ) ) { // only the table3 5 6's member can be a operand
						  if ( operand1 == "" ) operand1 = token;
						  else                  operand2 = token;
					  } // else if()
					  else if ( tableNo == 6 || tableNo == 7 ) StringOrDigitTransToMachineCode_newPC(token, file);

					  token = "";
				  } // if()

				  if ( isInstruction ) InstructionTransToMachineCode_newPC(instruction, operand1, operand2, file, -1);
					
		      finalOutput.push_back(temp);
		      instruction = operand1 = operand2 = "";
		      getFirstToken = isInstruction = false;
		    } // if()
			} // while(getline(inf, line))
	  } // Token()

		bool isAlphabet(char ch) { // Alphabet = a ~ z, A ~ Z
			if ( 'a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z' ) return true;
			else                                                    return false;
		} // isisAlphabet()
		
		bool isDigit(char ch) { // Digit = 0 ~ 9, A ~ H
			if ( '0' <= ch && ch <= '9' || 'A' <= ch && ch <= 'H' ) return true;
			else                                                    return false;
		} // isDigit()
		
		bool isTokensSame(string token, char ch) {
			if      ( isAlphabet(token[0]) && isAlphabet(ch) ) return true;
			else if ( isDigit(token[0]) && isDigit(ch) )       return true;
			else if ( isString && ch != 39 )                   return true; // ASCII 39 = '
			else                                               return false;
		} // isTokensSame()
		
		int findTokensHouse_whichTable(string token, File* file) {
		
			if ( token == "'" ) {
				if ( !isString ) isString = true;
				else             isString = false;
				  
				return 4;
	    } // if()
			else { // go to the vector of all table, try to find this token's type
			  for ( int a = 1 ; a <= 7 ; a++ )
				  for ( int b = 1 ; b < file->allTable[a].tokens.size() ; b++ )
					  if ( token == file->allTable[a].tokens[b].label ||                                         // ex : token = MOV
						    1 <= a && a <= 3 && token == TransToSmallLetter(file->allTable[a].tokens[b].label ) )  //      token = mov
						  return a; // we find it

				return findHash(token, file) / 100;  // we don'y find it in teble, so it is a first appear token, and it's type is 5 || 6 || 7
			} // else                                 we need to Hash() it, and put it in table.
		} // findTokensHouse_whichTable()
		
		int TransToAscii(string token) { // "Green " = 71 + 114 + 101 + 101 + 110 + 32
			int re = 0;
			for ( int a = 0 ; a < token.size() ; a++ ) re += token[a];
	    return re;
		} // TransToAscii()

		string TransToSmallLetter(string token) { // MOV -> mov
			for ( int a = 0 ; a < token.size() ; a++ ) token[a] += 32;
			return token;
		} // TransToSmallLetter()

		int String10ToInt16(string s) { // input is string, "10"(base16) -> 16(base10), return is interger
			int re = 0;
			for ( int a = 0 ; a < s.length() ; a++ ) re = re * 16 + (s[a] - '0');
			return re;
		} // StringToInt16()

		int findHash(string token, File* file) {
			int re, indexOfTable = TransToAscii(token) % 100;
			bool tokenAllIsDigit = true;
			
			if ( !isStartingAddressSet && isDigit(token[0]) && token[token.length() - 1] != 'H' ) { // set the start address
				for ( int a = 0 ; a < token.length() ; a++ ) if ( !isDigit(token[a]) ) tokenAllIsDigit = false;
				
				if ( tokenAllIsDigit ) {
					pc = String10ToInt16(token);
					isStartingAddressSet = true;
					return 0;
				} // if (tokenAllIsDigit)
			} // if()
			else if ( isString ) re = 7; // String
			else if ( token.length() >= 3 && isDigit(token[0]) && token[token.length() - 1] == 'H' ) re = 6; // digit
			else re = 5; // symbol, ex : Msg, DispMsg
				
			while (1) {
				if ( file->allTable[re].tokens[indexOfTable].label == "" ) { // if this site in table is null, we put the token in
					file->allTable[re].tokens[indexOfTable].label = token;
					return re * 100 + indexOfTable;
				} // if ()
				else indexOfTable = ( indexOfTable + 1 ) % 100; // if there isn't null, find the next site
			} // while (1)
		} // findHash()
		
		
		void StringOrDigitTransToMachineCode_newPC(string s, File* file) {
			if ( isDigit(s[0]) && s[s.size() - 1] == 'H' ) { // digit
				pc++, temp.machineCodeLength += s.size();
				temp.machineCode.push_back(s.substr(0, s.size() - 1));  // s = "0AH", then we push "0A" in vector
			} // if (isDigit)
			else {                                           // String
			  for ( int a = 0 ; a < s.size() ; a++ ) {
				  pc++, temp.machineCodeLength += 3;
				  temp.machineCode.push_back(CharToInt16_reString(s[a]));
			  } // for(a)
			} // else
		} // StringOrDigitTransToMachineCode_Print_newPC()
		
		void InstructionTransToMachineCode_newPC(string instruction, string operand1, string operand2, File* file, int siteAtFinalOutputVector) {
			if ( instruction == "INT" || instruction == "int" ) {
				pc++, temp.machineCodeLength += 3;
				temp.machineCode.push_back("CD");
				StringOrDigitTransToMachineCode_newPC(operand1, file);
			} // if ( INT )
			else if ( instruction == "RET" || instruction == "ret" ) {
				pc++, temp.machineCodeLength += 3;
				temp.machineCode.push_back("C3");
			} // else if ( RET )
			else if ( instruction == "CALL" || instruction == "call" ) {
				int symbolAddress = findAddress(operand1, file);
				
				if ( symbolAddress != -1 && symbolAddress != -2 ) { // this symbol has address
          if ( siteAtFinalOutputVector != -1 ) { // this Process is for forward reference
						finalOutput[siteAtFinalOutputVector].machineCode.push_back("E8");
						finalOutput[siteAtFinalOutputVector].machineCode.push_back(int10ToInt16_reSrting(findAddress(operand1, file) - finalOutput[siteAtFinalOutputVector].pc_forwardReference - 3, addTo4Digit_yes));
					} // if()
					else { // normal Process
						temp.machineCode.push_back("E8");
						temp.machineCode.push_back(int10ToInt16_reSrting(findAddress(operand1, file) - pc - 3, addTo4Digit_yes)); // address -> need to add '0'
					} // else
				} // if ()
				else if ( symbolAddress == -1 ) { // this symbol is forward reference
					temp.instruction_forwardReference = "CALL";
					temp.operand1_forwardReference = operand1;
					temp.pc_forwardReference = pc;
				} // else if ()
				
				pc += 3, temp.machineCodeLength += 8;
			} // else if ( CALL )
			else if ( instruction == "MOV" || instruction == "mov" ) {
				int operand1_type = findTokensHouse_whichTable(operand1, file), operand2_type = findTokensHouse_whichTable(operand2, file);
			
				if ( ( operand1 == "AX" || operand1 == "AH" || operand1 == "AL" ) && operand2_type == 5 ) { // Symbol to Accumulator
					if ( operand1 == "AX" ) temp.machineCode.push_back("A1"); // w = 1
					else                    temp.machineCode.push_back("A0"); // w = 0

					temp.machineCode.push_back(int10ToInt16_reSrting(findAddress(operand2, file), addTo4Digit_yes)); // address -> need to add '0'
					temp.machineCode.push_back("R");
					pc += 3, temp.machineCodeLength += 10;
				} // if (Symbol to Accumulator)
				else if ( operand1_type == 3 && operand2_type == 5 ) { // Symbol to Register
					int kindOfRegister = kindOfRegister_mov(operand1), machineCode_int10;
					
					if ( kindOfRegister % 10 == 0 ) temp.machineCode.push_back("8A");  // 1000|10dw, d = 1, w = 0 => 1000|1010 => 8A
					else                            temp.machineCode.push_back("8B");  // 1000|10dw, d = 1, w = 1 => 1000|1011 => 8B
					machineCode_int10 = kindOfRegister / 10 * 8 + 6;                   // 00|AAA|110(base2) is [0 + part A + 6](base10),
					                                                                   // 00 is symbol's mode, 110 is symbol's R/M
					temp.machineCode.push_back(int10ToInt16_reSrting(machineCode_int10, addTo4Digit_no)); // didn't need to add zero
					temp.machineCode.push_back(int10ToInt16_reSrting(findAddress(operand2, file), addTo4Digit_yes)); // address -> need to add '0'
					temp.machineCode.push_back("R");
					pc += 4, temp.machineCodeLength += 13;
				} // else if (Symbol to Register)
				else if ( ( operand1 == "CS" || operand1 == "DS" || operand1 == "ES" || operand1 == "SS" ) && operand2_type == 3 ) { // Register to segment Register
					int machineCode_int10 = 192; // 110|AA|BBB(base2) is [6 * 2^5 + part A + part B](base10), 11 is mode, 0 is stable
					int kindOfRegister1 = kindOfRegister_mov(operand1), kindOfRegister2 = kindOfRegister_mov(operand2);
					machineCode_int10 += kindOfRegister1 % 10 * 8 + kindOfRegister2 / 10;
					pc += 2, temp.machineCodeLength += 6;
					temp.machineCode.push_back("8E"); // the opcode is same
					temp.machineCode.push_back(int10ToInt16_reSrting(machineCode_int10, addTo4Digit_no)); // didn't need to add zero
				} // else if ( Register to segment Register )
				else if ( ( operand1 == "CS" || operand1 == "DS" || operand1 == "ES" || operand1 == "SS" ) && operand2_type == 5 ) { // Memory to segment Register
					; // I will add this case later
				} // else if ( Memory to segment Register )
				else if ( operand1_type == 3 && operand2_type == 6 ) { // Immediate to Register
					int machineCode_int10 = 176, kindOfRegister = kindOfRegister_mov(operand1);  // the opcode is 1011|A|BBB, A = w, B = register,
					                                                                             // 1011XXXX(base2) = 11 * 2^4 = 176(base10)
					if ( kindOfRegister % 10 == 1 ) machineCode_int10 += 8; // PART A, 1011|w|XXX if w = 1, 2^3 = 8
					machineCode_int10 += kindOfRegister / 10;               // part B
					temp.machineCode.push_back(int10ToInt16_reSrting(machineCode_int10, addTo4Digit_no));
					
					if ( machineCode_int10 >= 184 ) {
					  temp.machineCode.push_back(operand2.substr(2, 2)); // if w = 1, Immediate value is a word, ex : 1234H it's machine code
					  pc++, temp.machineCodeLength += 3;                 // should be 34 12.
					} // if ()
					
					temp.machineCode.push_back(operand2.substr(0, 2));                                 
					pc += 2, temp.machineCodeLength += 6;
				} // else if ( Immediate to Register )
			} // else if ( MOV )
		} // InstructionTransToMachineCode_Print_newPC()
		
		int kindOfRegister_mov(string rEgister) {
			int re;
			if      ( rEgister == "AL" || rEgister == "AX" ) re =  0;
			else if ( rEgister == "CL" || rEgister == "CX" ) re = 10;
			else if ( rEgister == "DL" || rEgister == "DX" ) re = 20;
			else if ( rEgister == "BL" || rEgister == "BX" ) re = 30;
			else if ( rEgister == "AH" || rEgister == "SP" ) re = 40;
			else if ( rEgister == "CH" || rEgister == "BP" ) re = 50;
			else if ( rEgister == "DH" || rEgister == "SI" ) re = 60;
			else if ( rEgister == "BH" || rEgister == "DI" ) re = 70;
			else if ( rEgister == "ES" || rEgister == "CS" ||
			          rEgister == "SS" || rEgister == "DS" ) re = 80;

			if      ( rEgister == "AL" || rEgister == "CL" || rEgister == "DL" || rEgister == "BL" ||
					      rEgister == "AH" || rEgister == "CH" || rEgister == "DH" || rEgister == "BH" )  re += 0;
			else if ( rEgister == "AX" || rEgister == "CX" || rEgister == "DX" || rEgister == "BX" ||
					      rEgister == "SP" || rEgister == "BP" || rEgister == "SI" || rEgister == "DI" )  re += 1;
			else if ( rEgister == "ES" )                                                              re += 0;
			else if ( rEgister == "CS" )                                                              re += 1;
			else if ( rEgister == "SS" )                                                              re += 2;
			else if ( rEgister == "DS" )                                                              re += 3;
			
			return re;
		} // kindOfRegister_mov()
		
		string int10ToInt16_reSrting(int n, bool addTo4Digit) { // input is int(base10) , 16(base10) -> "10"(base16), return is string(base16)
      int t;
      bool negative = false;
      if ( n < 0 ) negative = true;
			string re = "", s = "@";
			
			if ( !negative ) {
			  do {
          t = n % 16;

				  if ( 0 <= t && t <= 9 ) s[0] = t + '0';
          else                    s[0] = t - 10 + 'A';

				  re = s[0] + re, n /= 16;
        } while ( n );
        
        if ( addTo4Digit ) {
		  	  t = re.length();
		      for ( int a = 1 ; a <= 4 - t ; a++ ) re = "0" + re; // we have to add '0' to this string to 4 digits, ex : 4F -> 004F
		    } // if (addTo4Digit)
			} // if()
			else { // negative
				int binary[16] = {0}, negative_n = -n, carry = 15; // ex : -15 -> 15
				while ( negative_n > 0 ) {                         // 15 -> 00001111
					if ( negative_n % 2 == 1 ) binary[carry] = 1;
				  carry--, negative_n /= 2;
        } // while ( negative_n > 0 )

				for ( int a = 0 ; a < 16 ; a++ ) {                 // 00001111 -> 11110000 (1's Complement)
					if ( binary[a] == 0 ) binary[a] = 1;
				  else                  binary[a] = 0;
        } // for (a)

        carry = 1;                                         // 11110000 -> 11110001 (2's Complement)
				for ( int a = 15 ; a >= 0 ; a-- ) {
					if      ( binary[a] + carry == 1 ) binary[a] = 1, carry = 0;
					else if ( binary[a] + carry == 2 ) binary[a] = 0, carry = 1;
				} // for(a)
				
				for ( int a = 0 ; a <= 12 ; a += 4 ) {            // 11110001 -> F1 (base2 -> base16)
				  t = binary[a] * 8 + binary[a+1] * 4 + binary[a+2] * 2 + binary[a+3];
          if ( 0 <= t && t <= 9 ) s[0] = t + '0';
          else                    s[0] = t - 10 + 'A';
          re += s[0];
        } // for (a)
			} // else

      return re;
    } // int10ToInt16_reSrting()
		
		string CharToInt16_reString(char c) { // 'G' -> "47" (https://zh.wikipedia.org/wiki/ASCII), this function return string "47".
			int num1 = int(c) / 16, num2 = int(c) % 16;
			string snum1 = "@", snum2 = "@";
			
			snum1[0] = num1 + '0';
			if ( 0 <= num2 && num2 <= 9 ) snum2[0] = num2 + '0';
			else                          snum2[0] = num2 - 10 + 'A';
			
			return snum1 + snum2;
		} // CharToInt16_reString()
		
		int findAddress(string operand, File* file) {
			for ( int a = 0 ; a <= 99 ; a++ ) {
        if ( operand == file->allTable[5].tokens[a].label ) { // we can find this token in table, but maybe it doesn't have a address
				  if   ( file->allTable[5].tokens[a].address != -1 ) return file->allTable[5].tokens[a].address; // this token has address, return address(int10)
				  else                                               return -1;                                  // this token doesn't have address, return -1
				} // if()
      } // for (a)

			return -2;                                              // we don't find this token in table, return -2
		} // findAddress()
		
		void createOutputFile(File* file) {
			file->outputFileName = file->inputFileName;                             //    ex: input = 1.txt
			file->outputFileName.insert(file->inputFileName.size() - 4, "_output"); // then, output = 1_output.txt
			file->outf.open(file->outputFileName.c_str());

	    for ( int a = 0 ; a < finalOutput.size() ; a++ ) {
				if ( finalOutput[a].line_head != "" || finalOutput[a].machineCode.size() != 0 || finalOutput[a].instruction_forwardReference != "" )
				  file->outf << " " << std::hex << std::uppercase << right << setw(4) << setfill('0') << finalOutput[a].pc << "    ";
				else // this case didn't need to print pc and machinecode
					file->outf << "         "; // 9 ' '

				if ( finalOutput[a].instruction_forwardReference != "" )
					InstructionTransToMachineCode_newPC(finalOutput[a].instruction_forwardReference, finalOutput[a].operand1_forwardReference, finalOutput[a].operand2_forwardReference, file, a);
				
				for ( int b = 0 ; b <  finalOutput[a].machineCode.size()     ; b++ ) file->outf << finalOutput[a].machineCode[b] << " ";
				for ( int b = 1 ; b <= 26 - finalOutput[a].machineCodeLength ; b++ ) file->outf << " "; // Align

				file->outf << setw(13) << left << setfill(' ') << finalOutput[a].line_head << finalOutput[a].line_tail << "\n";
			} // for (a)
			
			file->outf.close();
    } // PrintFinalOutput()
    
    void clear_line_finalOutput(line_finalOutput &temp) {
			temp.instruction_forwardReference = temp.operand1_forwardReference = temp.operand2_forwardReference = temp.line_head = temp.line_tail = "";
			temp.machineCode.clear();
			temp.machineCodeLength = 0;
			temp.pc = temp.pc_forwardReference = -1;
		} // clear_line_finalOutput()
		
}; // class Token


int main() {
	File file;
	file.readTable("Table1.table");
  file.readTable("Table2.table");
	file.readTable("Table3.table");
	file.readTable("Table4.table");
	file.OpenInputFile();
  
	Token token(&file);
	token.createOutputFile(&file);
	
	cout << "Done ~";
} // main()
