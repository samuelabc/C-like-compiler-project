#ifndef CodeGen_h
#define CodeGen_h

#include"SymbolTable.h"
#include<string>
#include<vector>
#include<utility>
#include<map>
using std::pair;
using std::string;
using std::vector;
using std::map;
using std::make_pair;
using std::to_string;

class CodeGen {
public:
	CodeGen(vector<vector<string>> intermediateCodeVec, SymbolTable symbolTable);
	vector<string> textSegment;
	vector<string> dataSegment;
private:
	SymbolTable symbolTable;

	vector<vector<string>> intermediateCode;
	map<string, int> symbolsptable;
	int sp_offset;
	int strno;

	bool getNextIntermediateCode();
	bool retractIntermediateCode;

	int current_intermediate_line;
	vector<string> inter;

	string current_function;
	string current_function_return_type;

	map<string, vector<string>> global_const; //(name, (type,value))
	map<string, vector<string>> global_var; //(name, (type, value))
	map<string, vector<string>> global_array; //(name, (type, length, dim1, dim2))
	map<string, string> global_function; //(name, return type)

	map<string, string> local_parameter; //(name, type)
	map<string, vector<string>> local_const; //(name, (type,value))
	map<string, vector<string>> local_var; //(name, (type, value))
	map<string, vector<string>> local_array; //(name, (type, length, dim1, dim2))

	int push_count;	//parameter count
	vector<string> pushVec;

	vector<string> reg_v = { "$v0", "$v1" };
	vector<string> reg_a = { "$a0", "$a1", "$a2", "$a3" };
	vector<string> reg_t = { "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9" };
	vector<string> reg_s = { "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7"};

	string zero_value = "0";
	string reg_zero = "$zero";
	string stack_pointer = "$sp";
	string frame_pointer = "$fp";
	string return_address_pointer = "$ra";
	string global_pointer = "$gp";

	string global_function_name = "$GLOBAL";
	string program_end_label = "$end";

	string word_directive = ".word";
	string asciiz_directive = ".asciiz";
	string byte_directive = ".byte";
	

	void analyze();
	void stack_inc(int offset);
	void stack_dec(int offset);
	
	int findSpOfIdenfr(string name);
	bool isIntOrChar(string str);
	bool isArray(string str);
	string getType(string name);
	void loadArray(string arrName, string dest_reg);
	void storeArray(string arrName, string dest_reg);
	void load_value_into_reg(string dest_reg, string src);
	void prepare_parameter(string funcName);

	void set_word_directive(string wordName, string wordValue);
	void set_asciiz_directive(string str);
	void set_text_label(string funcName);
	void set_data_label(string labelName);
	void call_syscall();
	void store_global(string varName, string newValueReg);
	void load_global(string varName, string destReg);

	void move_instr(string dest, string src);
	void li_instr(string dest, string src);
	void la_instr(string dest, string src);
	void sw_instr(string value_reg, int offset_int, string baseAddress_reg);
	void lw_instr(string value_reg, int offset_int, string baseAddress_reg);
	void lw_instr(string value_reg,  string label);
	void jal_instr(string funcName);
	void j_instr(string dest);
	void jr_instr(string dest);
	void addiu_instr(string dest, string src1, int src2);
	void add_instr(string dest, string src1, string src2);	//addu
	void sub_instr(string dest, string src1, string src2);
	void mul_instr(string dest, string src1, string src2);
	void div_instr(string src1, string src2);
	void mflo_instr(string dest);
	void beq_instr(string src1, string src2, string label);
	void bne_instr(string src1, string src2, string label);
	void blt_instr(string src1, string src2, string label);
	void ble_instr(string src1, string src2, string label);
	void bgt_instr(string src1, string src2, string label);
	void bge_instr(string src1, string src2, string label);


	int frameSize;
	int paramsSize;
	int attrsSize; //#expr1, #expr2, func's attrs
	int tempSize; //#temp1 #temp2, ... ,#tempn
	int reg_other_Size; //$ra, $fp
	int reg_s_Size; //$s0-$s7
	int reg_t_Size; //$t0-$t9
	int callArgsSize;
	string dim1;
	string dim2;
	vector<string> dimVec;

	void returnLastFunction();
	void jumpToMainFunction();
	
};

#endif // !Code_Gen_h
