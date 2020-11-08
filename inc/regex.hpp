#include <stack>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <algorithm>
#include <queue>
#include <stdexcept>


namespace regex {
using std::queue;
using std::unordered_set;
using std::unordered_multimap;
using std::string;
using std::vector;
using std::stack;

class Regex {
    static const char EPS = '1';

    struct Node {
        int id;
        unordered_multimap<char, Node*> go;
        bool is_terminate;

        Node(bool is_terminate=false) : is_terminate(is_terminate), id(0) {}

        /* insert transition by char c to node next */
        void insert(char c, Node* next) {
            auto [start, end] = go.equal_range(c);
            if (find_if(start, end,
                        [&next] (const std::pair<const char, Node*>& elem) {
                            return elem.second == next;
                        }) == end)
                go.insert({c, next});
        }
    };

    class RegexPart {
        Node* start;
        Node* end;

        static RegexPart concat(RegexPart&& lhs, RegexPart&& rhs) {
            lhs.end->insert(EPS, rhs.start);
            RegexPart retval(lhs.start, rhs.end);

            lhs.start = lhs.end = nullptr;
            rhs.start = rhs.end = nullptr;

            return retval;
        }

        static RegexPart alternate(RegexPart&& lhs, RegexPart&& rhs) {
            Node* start = new Node;
            start->insert(EPS, lhs.start);
            start->insert(EPS, rhs.start);

            Node* end = new Node;
            lhs.end->insert(EPS, end);
            rhs.end->insert(EPS, end);

            lhs.start = lhs.end = nullptr;
            rhs.start = rhs.end = nullptr;

            return RegexPart(start, end);
        }

        static RegexPart asterisk(RegexPart&& lhs) {
            lhs.start->insert(EPS, lhs.end);
            lhs.end->insert(EPS, lhs.start);

            RegexPart retval(lhs.start, lhs.end);
            lhs.start = lhs.end = nullptr;

            return retval;
        }

        RegexPart(Node* start, Node* end) :
            start(start),
            end(end) {}

    public:
        RegexPart(char c) : start(new Node), end(new Node) {
            start->insert(c, end);
        }

        /* update stack st with operation op */
        static void apply(stack<RegexPart>& st, char op) {
            switch (op) {
                case '.': {
                    RegexPart rhs = st.top(); st.pop();
                    RegexPart lhs = st.top(); st.pop();
                    st.push(concat(std::move(lhs), std::move(rhs)));
                    break;
                }
                case '+': {
                    RegexPart rhs = st.top(); st.pop();
                    RegexPart lhs = st.top(); st.pop();
                    st.push(alternate(std::move(lhs), std::move(rhs)));
                    break;
                }
                case '*': {
                    RegexPart lhs = st.top(); st.pop();
                    st.push(asterisk(std::move(lhs)));
                    break;
                }
                default:
                    st.emplace(op);
                    break;
            }
        }

        /* get result */
        std::pair<Node*, Node*> get() {
            return {start, end};
        }
    };

    /* generate reachable nodes */
    static vector<Node*> gen_id2node(Node* start) {
        vector<Node*> ids;
        unordered_set<Node*> visited;

        stack<Node*> st;
        st.push(start);

        while(!st.empty()) {
            Node* current = st.top();
            st.pop();

            if (visited.count(current)) continue;
            visited.insert(current);
            ids.push_back(current);

            for (const auto& [_, next]: current->go)
                st.push(next);
        }

        return ids;
    }

    /* update terminateness and add transitions */
    static void update_transitions(Node* node) {
        unordered_set<Node*> visited;

        stack<Node*> st;
        st.push(node);

        while(!st.empty()) {
            Node* current = st.top();
            if (current->is_terminate)
                node->is_terminate = true;
            st.pop();

            if (visited.count(current)) continue;
            visited.insert(current);

            for (const auto& [c, next]: current->go)
                if (c == EPS) st.push(next);
                else node->insert(c, next);
        }
    }

    /* parse strint regex */
    static Node* construct(const string& regex) {
        stack<RegexPart> st;
        for (const char& op: regex)
            RegexPart::apply(st, op);

        if (st.size() != 1)
            throw std::invalid_argument("Invalid regex: " + regex);

        auto [start, end] = st.top().get();
        end->is_terminate = true;

        return start;
    }

    Node* root = nullptr;
    vector<Node*> id2node;

    Regex(Node* root, vector<Node*> id2node) : root(root), id2node(id2node) {}
public:

    static Regex from_string(const string& regex) {
        auto root = construct(regex);
        auto id2node = gen_id2node(root);

        for (const auto& node: id2node)
            update_transitions(node);
        for (const auto& node: id2node)
            node->go.erase(EPS);

        auto new_id2node = gen_id2node(root);
        unordered_set<Node*> reachable(new_id2node.begin(), new_id2node.end());
        for (const auto& node: id2node)
            if (!reachable.count(node))
                delete node;

        for (size_t id = 0; id < new_id2node.size(); ++id)
            new_id2node.at(id)->id = id;

        return Regex(root, new_id2node);
    }

    Regex(const Regex& other) = delete;

    Regex(Regex&& other) : root(other.root), id2node(other.id2node) {
        other.root = nullptr;
        other.id2node = vector<Node*>();
    }

    bool accept(const string& str) const {
        struct state {
            Node* node;
            size_t pos;
        };

        queue<state> bfs;
        bfs.push({root, 0});

        while (!bfs.empty()) {
            state current = bfs.front();
            bfs.pop();

            if (current.pos == str.size()) {
                if (current.node->is_terminate) return true;
                else continue;
            }

            const char& sc = str.at(current.pos);
            for (const auto& [c, next]: current.node->go)
                if (c == sc)
                    bfs.push({next, current.pos + 1});
        }

        return false;
    }

    static string large_acceptable_substring(const string& regex, const string& str) {
        Regex re = Regex::from_string(regex);

        for (size_t len = str.length(); len > 0; --len)
            for (size_t i = 0; i <= str.length() - len; ++i) {
                const string& substr = str.substr(i, len);
                if (re.accept(substr))
                    return substr;
            }

        return string();
    }

    ~Regex() {
        for (auto& node: id2node)
            delete node;
    }

    friend std::ostream& operator<<(std::ostream& out, const Regex& re);
};

const char Regex::EPS;

std::ostream& operator<<(std::ostream& out, const Regex& re) {
    out << "digraph automata {\n";
    for (const auto& node: re.id2node) {
        out << "node_" << node->id  << " [shape=";
        if (node->is_terminate) out << "box";
        else out << "circle";
        out << "]\n";
    }

    for (const auto& node: re.id2node)
        for (const auto& [c, next]: node->go)
            out << "node_" << node->id << " -> "
                << "node_" << next->id << " [label=\""
                << c << "\"]\n";

    out << "}";

    return out;
}

} // namespace Regex
