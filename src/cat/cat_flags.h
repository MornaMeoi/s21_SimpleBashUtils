#ifndef CAT_FLAGS_H
#define CAT_FLAGS_H

void CatSetTable(const char *table[static 256]);
void CatSetEndl(const char *table[static 256]);
void CatSetTab(const char *table[static 256]);
void CatSetNonPrintable(const char *table[static 256]);

#endif