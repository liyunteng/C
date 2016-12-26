#include <libxml/parser.h>
#include <libxml/xpath.h>

xmlDocPtr getdoc(char *docname)
{
	xmlDocPtr doc;
	doc = xmlParseFile(docname);
	 if (doc == NULL) {
		fprintf(stderr, "Document not parsed successfully.\n");
		return NULL;
	 }

	 return doc;
}
xmlXPathObjectPtr getnodeset(xmlDocPtr doc, xmlChar *xpath)
{
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;

	context = xmlXPathNewContext(doc);
	result = xmlXPathEvalExpression(xpath, context);

	if (xmlXPathNodeSetIsEmpty(result->nodesetval)) {
		printf("No result\n");
		return NULL;
	}
	xmlXPathFreeContext(context);
	return result;
}
int main(int argc,  char *argv[])
{
	char *docname;
	xmlDocPtr doc;
	xmlChar *xpath = ("//*");
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	int i;
	xmlChar *value;

	if (argc <= 1) {
		printf("Usage: %s docname\n", argv[0]);
		return 0;
	}

	docname = argv[1];
	doc = getdoc(docname);
	result = getnodeset(doc, xpath);

	if (result) {
		nodeset = result->nodesetval;
		for (i=0; i < nodeset->nodeNr; i++) {
			value = xmlNodeListGetString(doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
			printf("%s: %s\n", nodeset->nodeTab[i]->name, value);
			xmlFree(value);
		}
		xmlXPathFreeObject(result);
	}
	xmlFreeDoc(doc);
	return 0;
}
