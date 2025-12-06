#include <fstream>
#include <vector>

template <typename K, typename V>
struct KeyValue
{
    K key;
    V value;

    KeyValue(K k, V v) : key(k), value(v) {}
    bool operator<(const KeyValue& other) const
    {
        return key < other.key;
    }
};

template <typename K, typename V>
class BPlusNode
{
public:
    bool is_leaf;
    unsigned int size;
    unsigned int order;
    std::vector<K> keys;

    BPlusNode(bool leaf, unsigned int _order) : is_leaf(leaf), size(0), order(_order) {}
    virtual ~BPlusNode() = default;

    virtual void insert(K key, V value) = 0;
    virtual void split_child(unsigned int index, BPlusNode* child) = 0;
};

template <typename K, typename V>
class LeafNode : public BPlusNode<K, V>
{
public:
    std::vector<KeyValue<K, V>> data;
    LeafNode* next;

    LeafNode(unigned int order) : BPlusNode<K, V>(true, order), next(nullptr) {}

    void insert(K key, V value) override
    {
        auto it = std::lower_bound(data.begin(), data.end(),
            KeyValue<K, V>(key, value));
        data.insert(it, KeyValue<K, V>(key, value));
        this->keys.push_back(key);
        this->size++;
    }

    void split_child(int index, BPlusNode<K, V>* child) override {}
};

template <typename K, typename V>
class InternalNode : public BPlusNode<K, V>
{
public:
    std::vector<BPlusNode<K, V>*> children;

    InternalNode(unsigned int order) : BPlusNode<K, V>(false, order) {}

    ~InternalNode()
    {
        for (auto child : children)
        {
            delete child;
        }
    }

    void insert(K key, V value) override
    {
        int i = 0;
        while (i < this->keys.size() && key >= this->keys[i])
        {
            i++;
        }

        if (children[i]->size >= order - 1)
        {
            split_child(i, children[i]);
            if (key >= this->keys[i])
            {
                i++;
            }
        }

        children[i]->insert(key, value);
    }

    void split_child(int index, BPlusNode<K, V>* child) override
    {
        int mid = child->keys.size() / 2;
        K mid_key = child->keys[mid];

        if (child->is_leaf)
        {
            LeafNode<K, V>* leaf_child = static_cast<LeafNode<K, V>*>(child);
            LeafNode<K, V>* new_leaf = new LeafNode<K, V>();

            new_leaf->data.assign(leaf_child->data.begin() + mid,
                leaf_child->data.end());
            leaf_child->data.erase(leaf_child->data.begin() + mid,
                leaf_child->data.end());

            new_leaf->keys.assign(leaf_child->keys.begin() + mid,
                leaf_child->keys.end());
            leaf_child->keys.erase(leaf_child->keys.begin() + mid,
                leaf_child->keys.end());

            new_leaf->size = new_leaf->keys.size();
            leaf_child->size = leaf_child->keys.size();

            new_leaf->next = leaf_child->next;
            leaf_child->next = new_leaf;

            this->keys.insert(this->keys.begin() + index, new_leaf->keys[0]);
            children.insert(children.begin() + index + 1, new_leaf);
        }
        else
        {
            InternalNode* internal_child = static_cast<InternalNode*>(child);
            InternalNode* new_node = new InternalNode();

            new_node->keys.assign(internal_child->keys.begin() + mid + 1,
                internal_child->keys.end());
            internal_child->keys.erase(internal_child->keys.begin() + mid,
                internal_child->keys.end());

            new_node->children.assign(internal_child->children.begin() + mid + 1,
                internal_child->children.end());
            internal_child->children.erase(internal_child->children.begin() + mid + 1,
                internal_child->children.end());

            new_node->size = new_node->keys.size();
            internal_child->size = internal_child->keys.size();

            this->keys.insert(this->keys.begin() + index, mid_key);
            children.insert(children.begin() + index + 1, new_node);
        }

        this->size++;
    }
};

template <typename K, typename V>
class BPlusTree
{
private:
    BPlusNode<K, V>* root;
    LeafNode<K, V>* first_leaf;
    unsigned int order;
    unsigned int count;

public:
    BPlusTree(unsigned int _order) : root(nullptr), first_leaf(nullptr), order(_order) {}

    ~BPlusTree()
    {
        delete root;
    }

    void insert(K key, V value)
    {
        if (!root)
        {
            root = new LeafNode<K, V>(order);
            first_leaf = static_cast<LeafNode<K, V>*>(root);
        }

        if (root->size >= order - 1)
        {
            InternalNode<K, V>* new_root = new InternalNode<K, V>(order);
            new_root->children.push_back(root);
            new_root->split_child(0, root);
            root = new_root;
        }

        root->insert(key, value);
    }

    void serialize(std::fstream file)
    {

    }
};