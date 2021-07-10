#include"CodeGen.h"

void CodeGen::set_word_directive(string wordName, string wordValue) {
	dataSegment.push_back(word_directive + " " + wordValue);
}
void CodeGen::set_asciiz_directive(string str) {
	dataSegment.push_back(asciiz_directive + " " + str);
}
void CodeGen::move_instr(string dest, string src) {
	textSegment.push_back("move " + dest + " ," + src);
}
void CodeGen::li_instr(string dest, string src) {
	textSegment.push_back("li " + dest + " ," + src);
}
void CodeGen::la_instr(string dest, string src) {
	textSegment.push_back("la " + dest + " ," + src);
}
void CodeGen::sw_instr(string value_reg, int offset_int, string baseAddress_reg) {
	textSegment.push_back("sw " + value_reg + " ," + to_string(offset_int) + "(" + baseAddress_reg + ")");
}
void CodeGen::lw_instr(string value_reg, int offset_int, string baseAddress_reg) {
	textSegment.push_back("lw " + value_reg + " ," + to_string(offset_int) + "(" + baseAddress_reg + ")");
}
void CodeGen::lw_instr(string value_reg,string label) {
	textSegment.push_back("lw " + value_reg + " ," + label);
}
void CodeGen::jal_instr(string funcName) {
	textSegment.push_back("jal " + funcName);
}
void CodeGen::set_data_label(string labelName) {
	dataSegment.push_back("");	//skip one line
	dataSegment.push_back(labelName + ":");
}
void CodeGen::set_text_label(string labelName) {
	textSegment.push_back("");	//skip one line
	textSegment.push_back(labelName + ":");
}
void CodeGen::store_global(string varName, string newValueReg) {
	la_instr(reg_s[0], varName);
	sw_instr(newValueReg, 0, reg_s[0]);
}
void CodeGen::load_global(string varName, string destReg) {
	la_instr(reg_s[0], varName);
	lw_instr(destReg, 0, reg_s[0]);
}
void CodeGen::call_syscall() {
	textSegment.push_back("syscall");
}
void CodeGen::j_instr(string dest) {
	textSegment.push_back("j " + dest);
}
void CodeGen::jr_instr(string dest) {
	textSegment.push_back("jr " + dest);
}
void CodeGen::addiu_instr(string dest, string src1, int src2) {
	textSegment.push_back("addiu " + dest + ", " + src1 + ", " + to_string(src2));
}
void CodeGen::add_instr(string dest, string src1, string src2) {
	textSegment.push_back("addu " + dest + ", " + src1 + ", " + src2);
}
void CodeGen::sub_instr(string dest, string src1, string src2) {
	textSegment.push_back("sub " + dest + ", " + src1 + ", " + src2);
}
void CodeGen::mul_instr(string dest, string src1, string src2) {
	textSegment.push_back("mul " + dest + ", " + src1 + ", " + src2);
}
void CodeGen::div_instr(string src1, string src2) {
	textSegment.push_back("div " + src1 + ", " + src2);
}
void CodeGen::mflo_instr(string dest) {
	textSegment.push_back("mflo " + dest);
}
void CodeGen::beq_instr(string src1, string src2, string label) {
	textSegment.push_back("beq " + src1 + ", " + src2 + ", " + label);
}
void CodeGen::bne_instr(string src1, string src2, string label) {
	textSegment.push_back("bne " + src1 + ", " + src2 + ", " + label);
}
void CodeGen::blt_instr(string src1, string src2, string label) {
	textSegment.push_back("blt " + src1 + ", " + src2 + ", " + label);
}
void CodeGen::ble_instr(string src1, string src2, string label) {
	textSegment.push_back("ble " + src1 + ", " + src2 + ", " + label);
}
void CodeGen::bgt_instr(string src1, string src2, string label) {
	textSegment.push_back("bgt " + src1 + ", " + src2 + ", " + label);
}
void CodeGen::bge_instr(string src1, string src2, string label) {
	textSegment.push_back("bge " + src1 + ", " + src2 + ", " + label);
}




CodeGen::CodeGen(vector<vector<string>> intermediateCodeVec, SymbolTable symbolTable) {
	CodeGen::symbolTable = symbolTable;
	intermediateCode = intermediateCodeVec;
	dataSegment.push_back(".data");
	dataSegment.push_back("nextline: .asciiz \"\\n\"");
	textSegment.push_back(".text");
	strno = 0;
	sp_offset = 0;
	push_count = 0;
	dim1 = "-1";
	dim2 = "-1";
	retractIntermediateCode = false;
	current_intermediate_line = 0;
	current_function = global_function_name;
	analyze();
}
int CodeGen::findSpOfIdenfr(string name) {
	auto t = symbolsptable.find(name);
	if (t != symbolsptable.end()) { //found
		return t->second;
	}
	else { 
		return -1;
	}
}
void CodeGen::stack_inc(int offset) {
	addiu_instr(stack_pointer, stack_pointer, offset);
	/*sp_offset -= 4;*/
}
void CodeGen::stack_dec(int offset) {
	addiu_instr(stack_pointer, stack_pointer, offset);
	//sp_offset += 4;
}
void CodeGen::load_value_into_reg(string dest_reg, string src) {

}
void CodeGen::prepare_parameter(string funcName) {
	map<string, vector<vector<string>>> paramMap = symbolTable.funcParams;
	int paramNum = paramMap[funcName].size();
	push_count = paramNum - 1;

	for (int i = 0; i < paramNum; i++) {
		string param = pushVec.back();
		pushVec.pop_back();

		if (push_count < 4) {	//use a0-a3, store into stack also
			if (param[0] == '$') {
				string srcReg = param;
				move_instr(reg_a[push_count], srcReg);
			}
			else {
				string srcValue = param;
				if (isIntOrChar(srcValue)) {
					li_instr(reg_a[push_count], srcValue);
				}
				else {	//load var
					int offset = findSpOfIdenfr(srcValue);
					if (offset == -1) {	//global var
						load_global(srcValue, reg_a[push_count]);
					}
					else {	//local var
						lw_instr(reg_a[push_count], offset, stack_pointer);
					}
				}
			}
			sw_instr(reg_a[push_count], push_count * 4, stack_pointer); //store a0-a3 to stack also
		}
		else {	//save value of parameter to stack
			if (param[0] == '$') {
				string src_reg = param;
				int offset = 4 * push_count;
				string baseAddress_reg = stack_pointer;

				sw_instr(src_reg, offset, baseAddress_reg);	//store value of reg directly into stack
			}
			else {
				string src_value = param;
				string dest_reg = reg_t[0];

				if (isIntOrChar(src_value)) {
					li_instr(dest_reg, src_value);
				}
				else {	//load var
					int offset = findSpOfIdenfr(src_value);
					if (offset == -1) {	//global var
						load_global(src_value, dest_reg);
					}
					else {	//local var
						lw_instr(dest_reg, offset, stack_pointer);
					}
				}

				string src_reg = reg_t[0];
				int offset = 4 * push_count;
				string baseAddress_reg = stack_pointer;

				sw_instr(src_reg, offset, baseAddress_reg);
			}
		}
		push_count--;
	}
}


void CodeGen::loadArray(string arrName, string dest_reg) {
	int baseaddress = findSpOfIdenfr(arrName);
	if (baseaddress == -1) {	//global array
		vector<string> arrayInfo = global_array[arrName]; //(name, (type, length, dim1, dim2))
		if (arrayInfo.size() == 3) {	//dimension == 1
			int dimension1 = stoi(arrayInfo[2]);
			dim1 = dimVec.back();
			dimVec.pop_back();

			//realaddress = baseaddress + dim1 * 4;
			la_instr(reg_t[0], arrName);	//t0 is baseaddress
			if (isIntOrChar(dim1)) {
				li_instr(reg_t[1], dim1);	//t1 is dim1
			}
			else if (dim1[0] == '$') {
				move_instr(reg_t[1], dim1);	//t1 is dim1
			}
			else {
				int offset = findSpOfIdenfr(dim1);
				if (offset == -1) {
					lw_instr(reg_t[1], dim1);	//global var
				}
				else {	//local var
					lw_instr(reg_t[1], offset, stack_pointer);
				}
			}
			mul_instr(reg_t[1], reg_t[1], to_string(4));	//t1 = t1*4
			add_instr(reg_t[0], reg_t[0], reg_t[1]);	//t0 is realaddress
			lw_instr(dest_reg, 0, reg_t[0]);
		}
		else { //dimension == 2
			int dimension1 = stoi(arrayInfo[2]);
			int dimension2 = stoi(arrayInfo[3]);
			dim2 = dimVec.back();
			dimVec.pop_back();
			dim1 = dimVec.back();
			dimVec.pop_back();
			//realaddress = baseaddress + (dim1 * dimension2 + dim2) * 4;
			if (isIntOrChar(dim1)) {
				li_instr(reg_t[1], dim1);	//t1 is dim1
			}
			else if (dim1[0] == '$') {
				move_instr(reg_t[1], dim1);	//t1 is dim1
			}
			else {
				int offset = findSpOfIdenfr(dim1);
				if (offset == -1) {
					lw_instr(reg_t[1], dim1);	//global var
				}
				else {	//local var
					lw_instr(reg_t[1], offset, stack_pointer);
				}
			}
			mul_instr(reg_t[1], reg_t[1], to_string(dimension2));	//t1 = t1*dimension2
			
			if (!isIntOrChar(dim2) && dim2[0] != '$') {
				int offset = findSpOfIdenfr(dim2);
				if (offset == -1) {
					lw_instr(reg_t[0], dim2);	//global var
				}
				else {	//local var
					lw_instr(reg_t[0], offset, stack_pointer);
				}
				dim2 = reg_t[0];
			}
			add_instr(reg_t[1], reg_t[1], dim2); //t1 = t1 + dim2
			mul_instr(reg_t[1], reg_t[1], to_string(4));	//t1 = t1*4
			la_instr(reg_t[0], arrName);	//t0 is baseaddress
			add_instr(reg_t[0], reg_t[0], reg_t[1]);	//t0 is realaddress
			lw_instr(dest_reg, 0, reg_t[0]);
		}
	}
	else {	//local array
		vector<string> arrayInfo = local_array[arrName]; //(name, (type, length, dim1, dim2))
		if (arrayInfo.size() == 3) {	//dimension == 1
			int dimension1 = stoi(arrayInfo[2]);
			dim1 = dimVec.back();
			dimVec.pop_back();
			//realaddress = baseaddress + dim1 * 4;
			if (isIntOrChar(dim1)) {
				li_instr(reg_t[0], dim1);	//t0 is dim1
			}
			else if (dim1[0] == '$') {
				move_instr(reg_t[0], dim1);	//t0 is dim1
			}
			else {
				int offset = findSpOfIdenfr(dim1);
				if (offset == -1) {
					lw_instr(reg_t[0], dim1);	//global var
				}
				else {	//local var
					lw_instr(reg_t[0], offset, stack_pointer);
				}
			}
			mul_instr(reg_t[0], reg_t[0], to_string(4));	//t0 = t0*4
			add_instr(reg_t[0], reg_t[0], to_string(baseaddress));	//t0 is totalOffset
			add_instr(reg_t[0], reg_t[0], stack_pointer);	//t0 value of realaddress
			lw_instr(dest_reg, 0, reg_t[0]);
		}
		else { //dimension == 2
			int dimension1 = stoi(arrayInfo[2]);
			int dimension2 = stoi(arrayInfo[3]);
			dim2 = dimVec.back();
			dimVec.pop_back();
			dim1 = dimVec.back();
			dimVec.pop_back();
			//realaddress = baseaddress + (dim1 * dimension2 + dim2) * 4;
			if (isIntOrChar(dim1)) {
				li_instr(reg_t[0], dim1);	//t0 is dim1
			}
			else if (dim1[0] == '$') {
				move_instr(reg_t[0], dim1);	//t0 is dim1
			}
			else {
				int offset = findSpOfIdenfr(dim1);
				if (offset == -1) {
					lw_instr(reg_t[0], dim1);	//global var
				}
				else {	//local var
					lw_instr(reg_t[0], offset, stack_pointer);
				}
			}
			mul_instr(reg_t[0], reg_t[0], to_string(dimension2));	//t0 = t0*dimension2
			if (!isIntOrChar(dim2) && dim2[0] != '$') {
				int offset = findSpOfIdenfr(dim2);
				if (offset == -1) {
					lw_instr(reg_t[1], dim2);	//global var
				}
				else {	//local var
					lw_instr(reg_t[1], offset, stack_pointer);
				}
				dim2 = reg_t[1];
			}
			add_instr(reg_t[0], reg_t[0], dim2); //t0 = t0 + dim2
			mul_instr(reg_t[0], reg_t[0], to_string(4));	//t0 = t0*4

			add_instr(reg_t[0], reg_t[0], to_string(baseaddress));	//t0 is totalOffset

			add_instr(reg_t[0], reg_t[0], stack_pointer);	//t0 value of realaddress
			lw_instr(dest_reg, 0, reg_t[0]);
		}
	}
	dim1 = "-1";
	dim2 = "-1";
}
void CodeGen::storeArray(string arrName, string src_reg) {
	int baseaddress = findSpOfIdenfr(arrName);
	if (baseaddress == -1) {	//global array
		vector<string> arrayInfo = global_array[arrName]; //(name, (type, length, dim1, dim2))
		if (arrayInfo.size() == 3) {	//dimension == 1
			int dimension1 = stoi(arrayInfo[2]);
			dim1 = dimVec.back();
			dimVec.pop_back();
			//realaddress = baseaddress + dim1 * 4;
			la_instr(reg_t[0], arrName);	//t0 is baseaddress
			if (isIntOrChar(dim1)) {
				li_instr(reg_t[1], dim1);	//t1 is dim1
			}
			else if (dim1[0] == '$') {
				move_instr(reg_t[1], dim1);	//t1 is dim1
			}
			else {
				int offset = findSpOfIdenfr(dim1);
				if (offset == -1) {
					lw_instr(reg_t[1], dim1);	//global var
				}
				else {	//local var
					lw_instr(reg_t[1], offset, stack_pointer);
				}
			}
			mul_instr(reg_t[1], reg_t[1], to_string(4));	//t1 = t1*4
			add_instr(reg_t[0], reg_t[0], reg_t[1]);	//t0 is realaddress
			sw_instr(src_reg, 0, reg_t[0]);
		}
		else { //dimension == 2
			int dimension1 = stoi(arrayInfo[2]);
			int dimension2 = stoi(arrayInfo[3]);
			dim2 = dimVec.back();
			dimVec.pop_back();
			dim1 = dimVec.back();
			dimVec.pop_back();
			//realaddress = baseaddress + (dim1 * dimension2 + dim2) * 4;
			if (isIntOrChar(dim1)) {
				li_instr(reg_t[1], dim1);	//t1 is dim1
			}
			else if (dim1[0] == '$') {
				move_instr(reg_t[1], dim1);	//t1 is dim1
			}
			else {
				int offset = findSpOfIdenfr(dim1);
				if (offset == -1) {
					lw_instr(reg_t[1], dim1);	//global var
				}
				else {	//local var
					lw_instr(reg_t[1], offset, stack_pointer);
				}
			}
			mul_instr(reg_t[1], reg_t[1], to_string(dimension2));	//t1 = t1*dimension2

			if (!isIntOrChar(dim2) && dim2[0] != '$') {
				int offset = findSpOfIdenfr(dim2);
				if (offset == -1) {
					lw_instr(reg_t[0], dim2);	//global var
				}
				else {	//local var
					lw_instr(reg_t[0], offset, stack_pointer);
				}
				dim2 = reg_t[0];
			}
			add_instr(reg_t[1], reg_t[1], dim2); //t1 = t1 + dim2
			mul_instr(reg_t[1], reg_t[1], to_string(4));	//t1 = t1*4
			la_instr(reg_t[0], arrName);	//t0 is baseaddress
			add_instr(reg_t[0], reg_t[0], reg_t[1]);	//t0 is realaddress
			sw_instr(src_reg, 0, reg_t[0]);
		}
	}
	else {	//local array
		vector<string> arrayInfo = local_array[arrName]; //(name, (type, length, dim1, dim2))
		if (arrayInfo.size() == 3) {	//dimension == 1
			int dimension1 = stoi(arrayInfo[2]);
			dim1 = dimVec.back();
			dimVec.pop_back();
			//realaddress = baseaddress + dim1 * 4;
			if (isIntOrChar(dim1)) {
				li_instr(reg_t[0], dim1);	//t0 is dim1
			}
			else if (dim1[0] == '$') {
				move_instr(reg_t[0], dim1);	//t0 is dim1
			}
			else {
				int offset = findSpOfIdenfr(dim1);
				if (offset == -1) {
					lw_instr(reg_t[0], dim1);	//global var
				}
				else {	//local var
					lw_instr(reg_t[0], offset, stack_pointer);
				}
			}
			mul_instr(reg_t[0], reg_t[0], to_string(4));	//t0 = t0*4
			add_instr(reg_t[0], reg_t[0], to_string(baseaddress));	//t0 is totalOffset
			add_instr(reg_t[0], reg_t[0], stack_pointer);	//t0 value of realaddress
			sw_instr(src_reg, 0, reg_t[0]);
		}
		else { //dimension == 2
			int dimension1 = stoi(arrayInfo[2]);
			int dimension2 = stoi(arrayInfo[3]);
			dim2 = dimVec.back();
			dimVec.pop_back();
			dim1 = dimVec.back();
			dimVec.pop_back();
			//realaddress = baseaddress + (dim1 * dimension2 + dim2) * 4;
			if (isIntOrChar(dim1)) {
				li_instr(reg_t[0], dim1);	//t0 is dim1
			}
			else if (dim1[0] == '$') {
				move_instr(reg_t[0], dim1);	//t0 is dim1
			}
			else {
				int offset = findSpOfIdenfr(dim1);
				if (offset == -1) {
					lw_instr(reg_t[0], dim1);	//global var
				}
				else {	//local var
					lw_instr(reg_t[0], offset, stack_pointer);
				}
			}
			mul_instr(reg_t[0], reg_t[0], to_string(dimension2));	//t0 = t0*dimension2
			if (!isIntOrChar(dim2) && dim2[0] != '$') {
				int offset = findSpOfIdenfr(dim2);
				if (offset == -1) {
					lw_instr(reg_t[1], dim2);	//global var
				}
				else {	//local var
					lw_instr(reg_t[1], offset, stack_pointer);
				}
				dim2 = reg_t[1];
			}
			add_instr(reg_t[0], reg_t[0], dim2); //t0 = t0 + dim2
			mul_instr(reg_t[0], reg_t[0], to_string(4));	//t0 = t0*4

			add_instr(reg_t[0], reg_t[0], to_string(baseaddress));	//t0 is totalOffset

			add_instr(reg_t[0], reg_t[0], stack_pointer);	//t0 value of realaddress
			sw_instr(src_reg, 0, reg_t[0]);
		}
	}
	dim1 = "-1";
	dim2 = "-1";
}
bool CodeGen::isIntOrChar(string str) {
	if (str[0] == '\'' || str[0] == '-' || str[0] == '+' || (str[0] >= '0' && str[0] <= '9')) {
		return true;
	}
	return false;
}
bool CodeGen::isArray(string str) {
	auto t = local_array.find(str);
	if (t != local_array.end()) {
		return true;
	}
	t = global_array.find(str);
	if (t != global_array.end()) {
		return true;
	}
	return false;
}
string CodeGen::getType(string name) {	//will only use to get type of parameter and var
	string type;
	map<string, string>::iterator it1;
	map<string, vector<string> >::iterator it2;

	it1 = local_parameter.find(name);
	if (it1 != local_parameter.end()) {
		type = it1->second;
		return type;
	}

	it2 = local_var.find(name);
	if (it2 != local_var.end()) {
		type = it2->second[0];
		return type;
	}

	it2 = global_var.find(name);
	if (it2 != global_var.end()) {
		type = it2->second[0];
		return type;
	}
	return "error";
}
bool CodeGen::getNextIntermediateCode() {
	if (retractIntermediateCode) {
		retractIntermediateCode = false;
		return true;
	}
	else if (current_intermediate_line < intermediateCode.size()) {
		inter = intermediateCode[current_intermediate_line];
		current_intermediate_line++;
		return true;
	}
	else {
		inter = vector<string>{""};	//inter[0] is an empty string 
		return false;
	}
}
void CodeGen::returnLastFunction() {
	set_text_label("$return" + current_function);

	lw_instr(return_address_pointer, frameSize - 4, stack_pointer);
	lw_instr(frame_pointer, frameSize - 8, stack_pointer);
	stack_dec(frameSize);
	jr_instr(return_address_pointer);

	cout << current_function << endl;
	for (auto it = symbolsptable.begin(); it != symbolsptable.end(); ++it)	//print symbolsptable
	{
		cout << it->first << " " << it->second << endl;
	}
	cout << endl; 
	symbolsptable.clear();
	local_parameter.clear();
	local_const.clear();
	local_var.clear();
	local_array.clear();
}
void CodeGen::jumpToMainFunction() {
	jal_instr("main");
	j_instr(program_end_label);
}

void CodeGen::analyze() {
	getNextIntermediateCode();
	while (inter[0] == "CONST") {//CONST INTTK x = 10 
		string constName = inter[2];
		string constValue = inter[4];
		string constType = inter[1];

		global_const[constName] = vector<string>({ constType, constValue });
		set_data_label(constName);
		set_word_directive(constName, constValue);

		getNextIntermediateCode();
	}
	while (inter[0] == "VAR" || inter[0] == "ARRAY") {
		if (inter[0] == "VAR") {  //VAR INTTK a = 5 
			bool init = (inter.size() >= 4) ? true : false;
			string varName = inter[2];
			string varType = inter[1];
			string varValue = (init == true) ? inter[4] : zero_value;

			global_var[varName] = vector<string>({ varType, varValue });
			set_data_label(varName);
			set_word_directive(varName, varValue);
		}
		else if (inter[0] == "ARRAY") { //ARRAY 1 INIT INTTK arr 3 4 5 6 
			bool init = inter[2] == "INIT" ? true : false;
			string arrayName = inter[4];
			string arrayType = inter[3];
			int dimension = stoi(inter[1]);
			int dimension1 = 1;
			int dimension2 = 1;

			if (dimension == 1) {
				dimension1 = stoi(inter[5]);
			}
			else {
				dimension1 = stoi(inter[5]);
				dimension2 = stoi(inter[6]);
			}

			set_data_label(arrayName);
			int len = dimension1 * dimension2;
			if (dimension == 1) {
				global_array[arrayName] = vector<string>{ arrayType, to_string(len),to_string(dimension1) };
			}
			else {	//dimension == 2
				global_array[arrayName] = vector<string>{ arrayType, to_string(len),to_string(dimension1) ,to_string(dimension2) };
			}
			if (init) {
				int initStartIdx = (dimension == 1) ? 6 : 7;
				for (int j = initStartIdx; j < inter.size(); j++) {
					string value = inter[j];
					set_word_directive(arrayName, value);
				}
			}
			else {
				for (int j = 1; j <= dimension1 * dimension2; j++) {
					set_word_directive(arrayName, zero_value);
				}
			}
		}
		getNextIntermediateCode();
	}
	do {
		if (inter[0] == "FUNCTION") {
			if (current_function != global_function_name) {	//the function before this function ends, so return
				returnLastFunction();
			}
			else {
				jumpToMainFunction();
			}
			string funcName = inter[1];
			string funcReturnType = inter[2];

			set_text_label(funcName);

			current_function = funcName;
			current_function_return_type = funcReturnType;
			global_function[funcName] = funcReturnType;

			int paramsNum = symbolTable.funcParams[funcName].size();
			int attrsNum = 2;
			int tempNum = symbolTable.funcTempMax[funcName] + 1;
			vector<vector<string>> attrVec = symbolTable.funcAttrs[funcName];
			for (int i = 0; i < attrVec.size(); i++) {
				attrsNum += stoi(attrVec[i][3]); //get size of each attrs, including const, var, array
			}
			int callArgsNum = 4;	//4 place for arguments is always reserved
			vector<pair<string, int>> callVec = symbolTable.funcCalls[funcName];
			for (int i = 0; i < callVec.size(); i++) {
				if (callArgsNum < callVec[i].second) {
					callArgsNum = callVec[i].second;
				}
			}
			
			paramsSize = paramsNum * 4;
			attrsSize = attrsNum * 4; //#expr1, #expr2, func's attrs
			tempSize = tempNum * 4;
			reg_other_Size = 2 * 4; //$ra, $fp
			reg_s_Size = 8 * 4; //$s0-$s7
			reg_t_Size = 10 * 4; //$t0-$t9
			callArgsSize = callArgsNum * 4;
			

			/*cout << "funcName " << funcName << endl;
			cout << "paramsNum " << paramsNum << endl;
			cout << "paramsSize " << paramsSize << endl;
			cout << "attrsNum " << attrsNum << endl;
			cout << "attrsSize " << attrsSize << endl; 
			cout << "callArgsNum " << callArgsNum << endl;
			cout << "callArgsSize " << callArgsSize << endl;
			cout << endl;*/

			frameSize = attrsSize + tempSize + reg_other_Size + reg_s_Size + reg_t_Size + callArgsSize;
			//sp_offset = 0;

			stack_inc(-frameSize);	//stack pointer move downward for size of framesize
			sw_instr(return_address_pointer, frameSize - 4, stack_pointer);	//save ra
			sw_instr(frame_pointer, frameSize - 8, stack_pointer);	//save old(caller) fp
			addiu_instr(frame_pointer, stack_pointer, frameSize - 4);	//move fp
			
			//for (int i = 0; i <= 7; i++) {
			//	sw_instr(reg_s[i], frameSize - 8 - (4*(8-i)), stack_pointer);	//save sw
			//}

			getNextIntermediateCode();

			int parameter_count = 0;
			while (inter[0] == "PARA") {	//PARA INTTK p1 
				int address = frameSize + parameter_count * 4;
				parameter_count++;

				string paraName = inter[2];
				string paraType = inter[1];

				symbolsptable[paraName] = address;

				local_parameter[paraName] = paraType;

				getNextIntermediateCode();
			}

			int address_before_call = reg_t_Size + callArgsSize;
			for (int i = 0; i < tempNum; i++) {
				string varName = "#temp" + to_string(i);
				string varType = "INTTK";
				string varValue = "0";
				symbolsptable[varName] = address_before_call + i * 4;
				local_var[varName] = vector<string>({ varType, varValue });
			}

			symbolsptable["#expr1"] = address_before_call + tempSize;
			symbolsptable["#expr2"] = address_before_call + tempSize + 4;
			local_var["#expr1"] = vector<string>({ "INTTK", "0" });
			local_var["#expr2"] = vector<string>({ "INTTK", "0" });

			int attrCount = 2 + tempNum;	//already insert #expr1, #expr2, maybe #temp if needed

			while (inter[0] == "CONST") {//CONST INTTK x = 10 
				int address = address_before_call + attrCount * 4;
				attrCount++;

				string constName = inter[2];
				string constValue = inter[4];
				string constType = inter[1];

				local_const[constName] = vector<string>({ constType, constValue });
				symbolsptable[constName] = address;

				li_instr(reg_t[0], constValue);
				sw_instr(reg_t[0], address, stack_pointer);

				getNextIntermediateCode();
			}
			while (inter[0] == "VAR" || inter[0] == "ARRAY") {
				if (inter[0] == "VAR") {//VAR INTTK a = 5 
					int address = address_before_call + attrCount * 4;
					attrCount++;

					bool init = (inter.size() >= 4) ? true : false;
					string varName = inter[2];
					string varType = inter[1];
					string varValue = (init == true) ? inter[4] : reg_zero;
					local_var[varName] = vector<string>({ varType, varValue });
					symbolsptable[varName] = address;

					if (varValue == reg_zero) {
						sw_instr(reg_zero, address, stack_pointer);
					}
					else {
						li_instr(reg_t[0], varValue);
						sw_instr(reg_t[0], address, stack_pointer);
					}
				}
				else {//ARRAY 1 INIT INTTK arr 3 4 5 6 
					bool init = inter[2] == "INIT" ? true : false;
					string arrayName = inter[4];
					string arrayType = inter[3];
					int dimension = stoi(inter[1]);
					int dimension1 = 1;
					int dimension2 = 1;

					if (dimension == 1) {
						dimension1 = stoi(inter[5]);
					}
					else {
						dimension1 = stoi(inter[5]);
						dimension2 = stoi(inter[6]);
					}
					int len = dimension1 * dimension2;
					if (dimension == 1) {
						local_array[arrayName] = vector<string>{ arrayType, to_string(len),to_string(dimension1) };
					}
					else {	//dimension == 2
						local_array[arrayName] = vector<string>{ arrayType, to_string(len),to_string(dimension1) ,to_string(dimension2) };
					}
					symbolsptable[arrayName] = address_before_call + attrCount * 4;
					if (init) {
						int initStartIdx = (dimension == 1) ? 6 : 7;
						for (int j = initStartIdx; j < inter.size(); j++) {
							int address = address_before_call + attrCount * 4;
							attrCount++;

							string value = inter[j];
							li_instr(reg_t[0], value);
							sw_instr(reg_t[0], address, stack_pointer);
						}
					}
					else {
						attrCount += len;
					}
				}
				getNextIntermediateCode();
			}
			retractIntermediateCode = true; //retract inter
		}
		else if (inter[0] == "DIM") { //DIM i //DIM 1 // DIM $t0
			dimVec.push_back(inter[1]);
		}
		else if (inter[0] == "LOADARR") {
			string arrName = inter[2];
			string dest_reg = inter[1];
			loadArray(arrName, dest_reg);
		}
		/*else if (inter[0] == "STOREARR") {
			string arrName = inter[1];
			string src_reg = inter[2];
			storeArray(arrName, src_reg);
		}*/
		else if (inter[0] == "RET") {
			if (inter.size() == 2) {	//have return argument
				string name = inter[1];
				int offset = findSpOfIdenfr(name);
				if (offset == -1) {
					la_instr(reg_t[0], name);
				}
				else {
					lw_instr(reg_t[0], offset, stack_pointer);
				}
				move_instr(reg_v[0], reg_t[0]);
			}
			j_instr("$return" + current_function);
		}
		else if (inter[0] == "PUSH") { //PUSH 1   /  PUSH $t2
			pushVec.push_back(inter[1]);
		}
		else if (inter[0] == "CALL") { //CALL foo 
			string funcName = inter[1];

			prepare_parameter(funcName);

			for (int i = 0; i < reg_t.size(); i++) {		//store register t
				sw_instr(reg_t[i], callArgsSize + 4 * i, stack_pointer);
			}
			//for (int i = 0; i < reg_a.size(); i++) {//store register a
			//	sw_instr(reg_a[i], frameSize + 4 * i, stack_pointer);
			//}

			jal_instr(funcName);

			//for (int i = 0; i < reg_a.size(); i++) {//load register a
			//	lw_instr(reg_a[i], frameSize + 4 * i, stack_pointer);
			//}
			for (int i = 0; i < reg_t.size(); i++) {//load register t
				lw_instr(reg_t[i], callArgsSize + 4 * i, stack_pointer);
			}
		}

		else if (inter[0] == "LSS" || inter[0] == "LEQ" || inter[0] == "GRE" || inter[0] == "GEQ" || inter[0] == "EQL" || inter[0] == "NEQ") {	//EQL #expr1 #expr2 BF LABEL1
			//LSS #expr1 #expr2 BF LABEL0 
			if (inter[1][0] == '#') {
				int offset = findSpOfIdenfr(inter[1]); //inter[1] is  #expr1 ???
				lw_instr(reg_t[0], offset, stack_pointer);
			}
			if (inter[2][0] == '#') {
				int offset = findSpOfIdenfr(inter[2]); //inter[2] is  #expr2 ???
				lw_instr(reg_t[1], offset, stack_pointer);
			}
			else {	//inter[2] is constant
				li_instr(reg_t[1], inter[2]);
			}

			//LSS #expr1 #expr2 BF LABEL0 
			string left = reg_t[0];
			string right = reg_t[1];
			string label = inter[4];
			if (inter[3] == "BT") { //branch if expression is true
				if (inter[0] == "EQL") {
					beq_instr(left, right, label);
				}
				else if (inter[0] == "NEQ") {
					bne_instr(left, right, label);
				}
				else if (inter[0] == "LSS") {
					blt_instr(left, right, label);
				}
				else if (inter[0] == "LEQ") {
					ble_instr(left, right, label);
				}
				else if (inter[0] == "GRE") {
					bgt_instr(left, right, label);
				}
				else if (inter[0] == "GEQ") {
					bge_instr(left, right, label);
				}
			}
			else if (inter[3] == "BF") { //branch if expression is false
				if (inter[0] == "EQL") {
					bne_instr(left, right, label);
				}
				else if (inter[0] == "NEQ") {
					beq_instr(left, right, label);
				}
				else if (inter[0] == "LSS") {
					bge_instr(left, right, label);
				}
				else if (inter[0] == "LEQ") {
					bgt_instr(left, right, label);
				}
				else if (inter[0] == "GRE") {
					ble_instr(left, right, label);
				}
				else if (inter[0] == "GEQ") {
					blt_instr(left, right, label);
				}
			}
		}
		else if (inter[0] == "SET") {	//SET LABEL1 
			string labelName = inter[1];
			set_text_label(labelName);
		}
		else if (inter[0] == "GOTO") {	//GOTO LABEL0
			string labelName = inter[1];
			j_instr(labelName);
		}
		else if (inter[0] == "PRINTF") {
			//sw_instr(reg_a[0], frameSize, stack_pointer);	//store a0

			if (inter[1][0] == '#') {	//print term only //PRINTF #expr1 INTTK 
				string varName = inter[1];
				string varType = inter[2];

				int offset = findSpOfIdenfr(varName);
				lw_instr(reg_a[0], offset, stack_pointer);
				if (varType == "INTTK") {
					li_instr(reg_v[0], to_string(1));
				}
				else if (varType == "CHARTK") {
					li_instr(reg_v[0], to_string(11));
				}
				call_syscall();

			}
			else {	//print string //PRINTF "number of b is " #expr1 INTTK 
				string str = inter[1];
				string strname = "printstr" + to_string(strno);
				strno++;

				set_data_label(strname);
				set_asciiz_directive(str);

				la_instr(reg_a[0], strname);
				li_instr(reg_v[0], to_string(4));
				call_syscall();

				if (inter.size() >= 4) {	//print term //PRINTF "number of b is " #expr1 INTTK
					string varName = inter[2];
					string varType = inter[3];

					int offset = findSpOfIdenfr(varName);
					if (offset == -1) {
						lw_instr(reg_a[0], varName);
					}
					else {
						lw_instr(reg_a[0], offset, stack_pointer);
					}
					if (varType == "INTTK") {
						li_instr(reg_v[0], to_string(1));
					}
					else if (varType == "CHARTK") {
						li_instr(reg_v[0], to_string(11));
					}
					call_syscall();
				}
			}
			la_instr(reg_a[0], "nextline");
			li_instr(reg_v[0], to_string(4));
			call_syscall();

			//lw_instr(reg_a[0], frameSize, stack_pointer);	//recover a0
		}
		else if (inter[0] == "SCANF") {	//SCANF vara
			string varName = inter[1];
			string type = getType(varName);
			int offset = findSpOfIdenfr(varName);
			
			if (type == "INTTK") {
				li_instr(reg_v[0], to_string(5));
			}
			else { //type == "CHARTK") 
				li_instr(reg_v[0], to_string(12));
			}
			call_syscall();
			if (offset == -1) {
				store_global(varName, reg_v[0]);
			}
			else {
				sw_instr(reg_v[0], offset, stack_pointer);	//store read value into vara
			}
		}
		else if (inter[0] == "ASSIGN") { // ASSIGN #expr1 = b 
			string left = inter[1];
			string right = inter[3];
			if (left[0] == '$' && right[0] == '$') { //ASSIGN $t1 = $v0
				move_instr(left, right);
			}
			else if (left[0] == '$' && right[0] != '$') {
				if (isIntOrChar(right)) {//ASSIGN $t1 = 1
					li_instr(left, right);
				}
				else {	//ASSIGN $t1 = a
					int offset = findSpOfIdenfr(right);
					if (offset == -1) {
						lw_instr(left, right);
					}
					lw_instr(left, offset, stack_pointer);
				}
			}
			else if (left[0] != '$') {// ASSIGN #expr1 = b 
				int offsetl = findSpOfIdenfr(left);
				if (isArray(left)) { //right must be register, use s0
					if (isIntOrChar(right)) {
						li_instr(reg_s[0], right);
						right = reg_s[0];
					}
					else if (right[0] != '$') {
						int offset = findSpOfIdenfr(right);
						if (offset == -1) { //global var
							load_global(right, reg_s[0]);
						}
						else {
							lw_instr(reg_s[0], offset, stack_pointer);
						}
						right = reg_s[0];
					}
					storeArray(left, right);
					continue;
				}
				if (right[0] == '$') {// ASSIGN #expr1 = $t1
					if (offsetl == -1) {
						store_global(left, right);
					}
					else {
						sw_instr(right, offsetl, stack_pointer);
					}
				}
				else { //right[0]!='$' // ASSIGN #expr1 = b 
					if (isIntOrChar(right)) {//ASSIGN a = 1
						li_instr(reg_t[0], right);
						if (offsetl == -1) { //left is global var
							store_global(left, reg_t[0]);
						}
						else { //left is local var
							sw_instr(reg_t[0], offsetl, stack_pointer);
						}
					}
					else {
						int offsetr = findSpOfIdenfr(right);
						if (offsetr == -1) {	//right is global var
							lw_instr(reg_t[0], right);
						}
						else {	//right is local var
							lw_instr(reg_t[0], offsetr, stack_pointer);
						}
						if (offsetl == -1) {
							store_global(left, reg_t[0]);
						}
						else {
							sw_instr(reg_t[0], offsetl, stack_pointer);
						}
					}
				}
			}
		}
		else if (inter[0] == "PLUS" || inter[0] == "MINU" || inter[0] == "MULT" || inter[0] == "DIV") {
			string destObj = inter[1];
			string oriDestObj = destObj;
			string left = inter[3];
			string right = inter[5];
			bool useTemp = false;
			if (destObj[0] != '$') {
				useTemp = true;
				int offset = symbolsptable[destObj];
				lw_instr(reg_s[1], offset, stack_pointer);
				destObj = reg_s[1];
			}
			if (left[0] != '$') {	//MINU $t2 = b - 10 
				if (isIntOrChar(left)) {	//PLUS $t2 = 1 + 2 
					li_instr(reg_t[0], left);
				}
				else {	//MINU $t2 = b - 10 
					int offset = findSpOfIdenfr(left);
					if (offset == -1) {
						lw_instr(reg_t[0], left);
					}
					else {
						lw_instr(reg_t[0], offset, stack_pointer);
					}
				}
				left = reg_t[0];
			}
			if (right[0] != '$') {	//MINU $t2 = b - 10 
				if (isIntOrChar(right)) {	//MINU $t2 = b - 10 
					li_instr(reg_t[1], right);
				}
				else {	//PLUS $t2 = 10 + a 
					int offset = findSpOfIdenfr(right);
					if (offset == -1) {
						lw_instr(reg_t[1], right);
					}
					else {
						lw_instr(reg_t[1], offset, stack_pointer);
					}
				}
				right = reg_t[1];
			}
			if (inter[0] == "PLUS") {
				add_instr(destObj, left, right);
			}
			else if (inter[0] == "MINU") {
				sub_instr(destObj, left, right);
			}
			else if (inter[0] == "MULT") {
				mul_instr(destObj, left, right);
			}
			else if (inter[0] == "DIV") {
				/*double tomult = 1.0 / stoi(inter[5]);
				string tomultstr = to_string(tomult);
				textSegment.push_back("mul .d " + inter[1] + ", " + left + ", " + tomultstr);*/
				div_instr(left, right);
				mflo_instr(destObj);
			}
			if (useTemp) {
				int offset = symbolsptable[oriDestObj];
				sw_instr(destObj, offset, stack_pointer);
			}
		}
	} while (getNextIntermediateCode());
	returnLastFunction(); //return main function
	set_text_label(program_end_label);
}
