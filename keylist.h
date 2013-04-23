/* kl is an ordered keyed linked list, keys are ints.
values can be retrieved, inserted, deleted by key */

/* ---- data structures ---- */

/* klnode is one of the nodes in a keyed ordered linked list */
typedef struct klnode {
    int             key;
    void            *value;
    struct klnode   *next;
} klnode;

/* kl is the header containing the pointer to the first node in the list
if the list is empty, the pointer is NULL */
typedef struct kl {
    struct klnode   *first;
} kl;

/* ---- public ---- */

/* create an empty kl */
kl *kl_create(void);

/* destroy a kl and all nodes it contains */
void kl_destroy(kl *list);

/* find a value in the kl by key, returning NULL if not found */
void *kl_find(const kl *list, int key);

/* remove a value from the kl by key, return 1 on success, 0 on failiure */
int kl_remove(kl *list, int key);

/* insert a value into the kl by key, updates if the key already exists */
void kl_insert(kl *list, int key, void *value);

/* display a kl */
void kl_output(const kl *list);

/* returns the number of nodes making up a kl */
int kl_len(const kl *list);

/* ---- private ---- */

static klnode *klnode_create(int key, void *value, klnode *next);

/* returns the address of the pointer to the node with the given key
or the node immediately after it */
static klnode **klnode_locate(const kl *list, int key);
