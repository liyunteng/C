#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

void getReference(xmlDocPtr doc, xmlNodePtr cur)
{
	xmlChar *uri;

	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"reference"))) {
			uri = xmlGetProp(cur, "uri");
			printf("uri: %s\n", uri);

			xmlFree(uri);
		}
		cur = cur->next;
	}
	return;
}

void parseDoc(char *docname)
{
	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile(docname);
	if (doc == NULL) {
		fprintf(stderr, "Document not parsed successfully.\n");
		return;
	}

	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		fprintf(stderr, "empty docment\n");
		xmlFreeDoc(doc);
		return;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *)"story")) {
		fprintf(stderr, "document of the wrong type, root node != story\n");
		xmlFreeDoc(doc);
		return;
	}

	getReference(doc, cur);
	xmlFreeDoc(doc);
	return;
}
int main(int argc, char *argv[])
{
	char *docname;
	if (argc <= 1) {
		printf("Usage: %s docname\n", argv[0]);
		return -1;
	}

	docname = argv[1];
	parseDoc(docname);
	return 0;
}
