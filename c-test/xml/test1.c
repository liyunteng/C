#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#define XML_CONF "./ata2slot.xml"

int main(int argc, char *argv[])
{
    xmlDocPtr doc;
    xmlNodePtr node;
    xmlNodePtr cur;
    int ata_low, ata_up, slot;
    char buf[10];
    xmlChar *xmlatalow, *xmlataup, *xmlslot;

    if ((doc = xmlReadFile(XML_CONF, "UTF-8", XML_PARSE_RECOVER)) == NULL)
	return -1;

    if ((node = xmlDocGetRootElement(doc)) == NULL)
	goto error_quit;

    node = node->xmlChildrenNode;
    memset(buf, 0, sizeof(buf));

    while (node) {
	if ((!xmlStrcmp(node->name, (const xmlChar *) "map")) &&
	    ((xmlatalow =
	      xmlGetProp(node, (const xmlChar *) "ata_lower")) != NULL)
	    &&
	    ((xmlataup =
	      xmlGetProp(node, (const xmlChar *) "ata_upper")) != NULL)
	    &&
	    ((xmlslot =
	      xmlGetProp(node, (const xmlChar *) "slot_lower")) != NULL)
	    ) {
	    ata_low = atoi((const char *) xmlatalow);
	    ata_up = atoi((const char *) xmlataup);
	    slot = atoi((const char *) xmlslot);
	    printf("slot:%d\tlow:%d\tup:%d\n", slot, ata_low, ata_up);

	}

	node = node->next;
    }


  error_quit:
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return 0;

}
