/* sl is a skiplist, keys are ints.
values can be retrieved, inserted, deleted by key */

/* ---- data structures ---- */

/* slnode is one of the nodes in a skiplist */
typedef struct slnode {
    int             key;
    void            *value;
    struct slnode   **next;
    int             level;
} slnode;

/* sl is the header containing the pointer to the first nodes in the list
if the list is empty, the pointer is NULL */
typedef struct sl {
    int             level; 
    struct slnode   **first;
} sl;

/* ---- public ---- */

/* create an empty sl */
sl *sl_create(void);

/* destroy a sl and all nodes it contains */
void sl_destroy(sl *list);

/* find a value in the sl by key, returning NULL if not found */
void *sl_find(const sl *list, int key);

/* remove a value from the sl by key, return 1 on success, 0 on failiure */
int sl_remove(sl *list, int key);

/* insert a value into the sl by key, updates if the key already exists */
void sl_insert(sl *list, int key, void *const value);

/* display a sl */
void sl_output(const sl *list);

/* returns the number of nodes making up a sl */
int sl_len(const sl *list);

/* ---- private ---- */

static int random_level(void);
static slnode *slnode_create(int key, void *const value);
static void slnode_destroy(slnode *node);
static slnode ***slnode_locate(sl *const list, int key);
/* returns the number of chars needed to represent the int in base 10 */
static int intstrlen(int x); 

#define SL_MAXLEVEL 20