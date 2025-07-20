#include <init/table.h>
#include <string.h>

int table_lookup(struct table_entry *out,
                 struct table       *table,
                 const char         *name,
                 size_t              namelen,
                 enum type           type) {
    for (size_t i = 0; i < table->num_entries; i++) {
        struct table_entry entry = table->entries[i];

        if (0 == strncmp(name, entry.name, namelen)) {
            if (type == entry.type || TYPE_ANY == type) {
                *out = entry;
                return LOOKUP_FOUND;
            }
        }
    }

    return LOOKUP_NOT_FOUND;
}
