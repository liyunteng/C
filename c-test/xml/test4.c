#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

void parseStory(xmlDocPtr doc, xmlNodePtr cur, char *keyword)
{
    xmlNewProp(xmlNewTextChild(cur, NULL, "lytlyt", keyword), "test",
               "ddd");

    return;
}

xmlDocPtr parseDoc(char *docname, char *keyword)
{
    xmlDocPtr doc;
    xmlNodePtr cur;

    doc = xmlParseFile(docname);
    if (doc == NULL) {
        fprintf(stderr, "Documents not parsed successfully.\n");
        return NULL;
    }

    cur = xmlDocGetRootElement(doc);
    if (cur == NULL) {
        fprintf(stderr, "empty documents\n");
        xmlFreeDoc(doc);
        return NULL;
    }

    if (xmlStrcmp(cur->name, (const xmlChar *) "story")) {
        fprintf(stderr, "document os the wrong type, root node != story");
        xmlFreeDoc(doc);
        return NULL;
    }

    cur = cur->xmlChildrenNode;
    while (cur) {
        if ((!xmlStrcmp(cur->name, (const xmlChar *) "storyinfo"))) {
            parseStory(doc, cur, keyword);
        }
        cur = cur->next;
    }

    return doc;
}

int main(int argc, char *argv[])
{
    char *docname;
    char *keyword;
    xmlDocPtr doc;

    if (argc <= 2) {
        printf("Usage: %s docname, keyword\n", argv[0]);
        return 0;
    }

    docname = argv[1];
    keyword = argv[2];
    doc = parseDoc(docname, keyword);

    if (doc != NULL) {
        xmlSaveFormatFile(docname, doc, 0);
        xmlFreeDoc(doc);
    }

    return 0;
}
