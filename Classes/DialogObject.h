#ifndef __DIALOG_OBJECT_H__
#define __DIALOG_OBJECT_H__

#define DIALOG_NODE_MAX_CHILD 255

#include "cocos2d.h"

// CAUTION : The dialog node don't know there parent. 
// So avoid trying to access it.
class DialogNode {
public:
	DialogNode();
	virtual ~DialogNode();

	static DialogNode* LoadXml(tinyxml2::XMLElement*);
	const char *pContent;
	DialogNode *pChild[DIALOG_NODE_MAX_CHILD];
	int numOfChild;
};

// The dialog object should be simple.
class DialogObject {
public:
	DialogObject();
	virtual ~DialogObject();

	static DialogObject * CreateNewDialog(char *pFileName);
	// Single statement : blablabla
	// Muti statement : blablabla;blablabla;blablabla;
	// The choice id is the position in the statement array.
	// It's also the position in the pChild array.
	const char * GetNextStatement(int choiceID);
	const char * rootName;
	DialogNode * pDialogTree;
	DialogNode * pCurrentNode;
	static char * ParseContent(const char * content, int contentSize);
};

unsigned int str2int(char const *s);

unsigned int UnicodeToUtf8(unsigned int input);

#endif