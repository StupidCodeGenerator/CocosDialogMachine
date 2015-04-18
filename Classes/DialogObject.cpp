#include "DialogObject.h"
#define COCOS2D_DEBUG 1;

USING_NS_CC;

DialogNode::DialogNode() {}

DialogNode::~DialogNode() {
	delete(this->pContent);
}

DialogObject::DialogObject() {}

DialogObject::~DialogObject() {}

DialogObject * DialogObject::CreateNewDialog(char *pFileName) {
	DialogObject * pResult = new DialogObject();
	std::string fullPath =
		CCFileUtils::sharedFileUtils()->fullPathForFilename(pFileName);

	tinyxml2::XMLDocument xDoc = new tinyxml2::XMLDocument();
	xDoc.LoadFile(fullPath.c_str());
	tinyxml2::XMLElement *rootEle = xDoc.RootElement();
	pResult->rootName = rootEle->FirstChildElement("p")->GetText();

	pResult->pDialogTree = DialogNode::LoadXml(
		rootEle->FirstChildElement("ul")->FirstChildElement("li"));
	// current node is the root when the dialog is built.
	pResult->pCurrentNode = pResult->pDialogTree;

	return pResult;
}


// The input must be an <li>
// Each <li> contains <ctnt> and <ul>, and <ul> contains <li>s
DialogNode * DialogNode::LoadXml(tinyxml2::XMLElement *xmlElement) {
	DialogNode *pResult = new DialogNode();
	// 1. Load root node's content <ctnt>
	// the GetText() function will destory the pointer points.
	// so we need to copy them out 
	// max string size is 200
	pResult->pContent = DialogObject::ParseContent(
		xmlElement->FirstChildElement("ctnt")->GetText(), 200);
	// 2. Load child if exist
	tinyxml2::XMLElement * pChildUL = xmlElement->FirstChildElement("ul");
	int childCount = 0;
	if (pChildUL != NULL) {
		tinyxml2::XMLElement *pChildLi = pChildUL->FirstChildElement("li");
		if (pChildLi != NULL) {
			pResult->pChild[childCount++] =
				DialogNode::LoadXml(pChildUL->FirstChildElement("li"));
			pChildUL->DeleteChild(pChildUL->FirstChildElement("li"));
		}

	}
	pResult->numOfChild = childCount;
	return pResult;
}

const char* DialogObject::GetNextStatement(int choiceID) {
	if (choiceID >= DIALOG_NODE_MAX_CHILD)
		return NULL;
	if (this->pCurrentNode == NULL)
		return NULL;

	const char *pResult = this->pCurrentNode->pContent;
	pCurrentNode = pCurrentNode->pChild[choiceID];
	return pResult;
}

// Make the content str into char array
// input : _20320;_20320
// result : 0x4F,0x60,0x4F,0x60
char * DialogObject::ParseContent(const char * content, int contentSize) {
	// move the character into saprate char arrays.
	// The chinese character will start with '_' and stop with ';'
	// The number in that is just unicode.
	// so i need a list to store that arrays.
	// I made an array list in java at first so maybe 
	// I can make one in cpp too.
	const int STATE_CHINESE_CHAR = 1;
	const int STATE_NORMAL = 0;
	int state = STATE_NORMAL;
	int index = 0;

	char * pResultContent = new char[contentSize];
	int resultContentIndex = 0;
	char * pChineseCharCodeBuf = new char[10];
	int chineseCharCodeBufIndex = 0;
	while (content[index] != '\0') // not the end
	{
		switch (state) {
		case STATE_NORMAL:
			if (content[index] == '_') {
				int i;
				state = STATE_CHINESE_CHAR;
				// Clear the chineseCharCodeBuf
				for (i = 0; i < 10; i++) {
					pChineseCharCodeBuf[i] = -1;
					chineseCharCodeBufIndex = 0;
				}
			} else
				pResultContent[resultContentIndex++] = content[index];
			break;
		case STATE_CHINESE_CHAR:
			if (content[index] == ';') {
				// Collect the characters in the buffer and translate them into
				// UNICODE chars. (2 chars);
				pChineseCharCodeBuf[chineseCharCodeBufIndex++] = '\0';
				unsigned int chineseUnicode = str2int(pChineseCharCodeBuf);
				unsigned int chineseUTF = UnicodeToUtf8(chineseUnicode);
				char c1 = chineseUTF >> 16;
				char c2 = (chineseUTF >> 8) & 0xFF;
				char c3 = chineseUTF & 0xFF;
				pResultContent[resultContentIndex++] = c1;
				pResultContent[resultContentIndex++] = c2;
				pResultContent[resultContentIndex++] = c3;
				state = STATE_NORMAL;
			} else {
				pChineseCharCodeBuf[chineseCharCodeBufIndex++] = content[index];
			}
			break;
		}
		index++;
	}
	pResultContent[resultContentIndex] = '\0';
	return pResultContent;
}

unsigned int str2int(char const *s) {
	unsigned int result = 0;

	// loop the char array and translate each char into numbers.
	int index = 0;
	while (s[index] != '\0') {
		index++;
	}
	index--;
	char numWeight = 0;
	while (index >= 0) {
		char num = s[index] - '0';
		result += num * pow(10, numWeight);
		numWeight++;
		index--;
	}

	return result;
}

int pow(int b, int i) {
	int result = 1;
	while (i > 0) {
		result *= b;
		i--;
	}
	return result;
}

// Unicode to UTF-8
// fill the bits into 'x's from right to left
// 0000 0000 - 0000 007F | 0xxxxxxx
// 0000 0080 - 0000 07FF | 110xxxxx 10xxxxxx
// 0000 0800 - 0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
// 0001 0000 - 0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
unsigned int UnicodeToUtf8(unsigned int input) {
	unsigned int part3, part2, part1, part0;
	unsigned int result3, result2, result1, result0;
	if (input < 0x7F) {
		return input;
	} else if (input < 0x7FF) {
		part0 = input & 0x3F; // 6 bits
		part1 = input >> 6; // remaining bits
		result0 = part0 | 0x80;
		result1 = part1 | 0xC0;
		return (result1 << 8) | result0;
	} else if (input < 0xFFFF) {
		part0 = input & 0x3F; // 6 bits
		part1 = (input & 0xFFF) >> 6; // 12 bits
		part2 = input >> 12; // remaining bits
		result0 = part0 | 0x80;
		result1 = part1 | 0x80;
		result2 = part2 | 0xE0;
		return (result2 << 16) | (result1 << 8) | result0;
	} else if (input < 0x10FFFF) {
		part0 = input & 0x3F; // 6 bits
		part1 = (input & 0xFFF) >> 6; // 12 bits
		part2 = (input & 0x3FFFF) >> 12; // 18 bits
		part3 = input >> 18; // remaining bits
		result0 = part0 | 0x80;
		result1 = part1 | 0x80;
		result2 = part2 | 0x80;
		result3 = part3 | 0xF0;
		return (result3 << 24) | (result2 < 16) | (result1 < 8) | result0;
	} else {
		return -1;
	}
}