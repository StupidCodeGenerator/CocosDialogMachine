import os
import math

fileNameList = os.listdir()

def HtmlToXml(fileName):
	fileObject = open(fileName, 'r')
	fileStr = fileObject.read()
	
	bodyStart = fileStr.index('<body>')
	bodyEnd = fileStr.index('</body>') + 7
	bodyStr = fileStr[bodyStart:bodyEnd]
	
	firstUlIndex = bodyStr.index('\n<ul>')
	bodyStr = bodyStr[:firstUlIndex] + '</p>' + bodyStr[firstUlIndex:]
	
	bodyStrLines = bodyStr.split("\n")
	bodyCount = len(bodyStrLines)
	for i in range(0, bodyCount):
		bodyStrLines[i] = bodyStrLines[i].replace("&#","_")
		if '<li>' in bodyStrLines[i]:
			liIndex = bodyStrLines[i].index('<li>')
			if liIndex > 0:
				bodyStrLines[i] = (bodyStrLines[i])[:(liIndex + 4)] + \
				'<ctnt>' + (bodyStrLines[i])[(liIndex + 4):] + '</ctnt>\n'
	
	targetFile = open(os.path.splitext(fileName)[0] + ".xml", 'w')
	targetFile.write("<?xml version=\"1.0\"?>\n<!DOCTYPE PLAY SYSTEM \"play.dtd\"\n>")
	targetFile.writelines(bodyStrLines)
	
	print ('Write file : ' + targetFile.name)
	
for fileNameStr in fileNameList:
	if os.path.splitext(fileNameStr)[1] == ".html":
		HtmlToXml(fileNameStr)