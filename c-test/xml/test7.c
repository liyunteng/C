#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
char *convert(char *, char *);
int main(int argc, char **argv)
{
    unsigned char *content, *out;
    xmlDocPtr doc;
    xmlNodePtr rootnode;
    char *encoding = "ISO-8859-1";
    if (argc <= 1) {
        printf("Usage: %s content\n", argv[0]);
        return (0);
    }
    content = argv[1];
    out = convert(content, encoding);
    doc = xmlNewDoc("1.0");
    rootnode = xmlNewDocNode(doc, NULL, (const xmlChar *) "root", out);
    xmlDocSetRootElement(doc, rootnode);
    xmlSaveFormatFileEnc("-", doc, encoding, 1);
    return (1);
}

char *convert(char *instr, char *encoding)
{
    xmlCharEncodingHandlerPtr handler;
    xmlBufferPtr in, out;
    handler = xmlFindCharEncodingHandler(encoding);
    if (NULL != handler) {
        in = xmlBufferCreate();
        xmlBufferWriteChar(in, instr);
        out = xmlBufferCreate();
        if (xmlCharEncInFunc(handler, out, in) < 0) {
            xmlBufferFree(in);
            xmlBufferFree(out);
            return NULL;
        } else {
            xmlBufferFree(in);
            return (char *) out->content;
        }
    }
}
