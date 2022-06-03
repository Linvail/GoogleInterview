#include "Graph.h"
#include "LeetCodeUtil.h"

#include <vector>
#include <string>
#include <set>
#include <stack>
#include <queue>
#include <deque>
#include <unordered_map>
#include <unordered_set>

namespace Graph
{
    using namespace std;

    //---------------------------------------------------------------------------------------
    // 399. Evaluate Division
    // You are given an array of variable pairs equations and an array of real numbers
    // values, where equations[i] = [Ai, Bi] and values[i] represent the equation Ai / Bi =
    // values[i]. Each Ai or Bi is a string that represents a single variable.
    //
    // You are also given some queries, where queries[j] = [Cj, Dj] represents the jth
    // query where you must find the answer for Cj / Dj = ? .
    //
    // Return the answers to all queries.If a single answer cannot be determined, return
    // -1.0.
    //
    // Note : The input is always valid.You may assume that evaluating the queries will not
    // result in division by zero and that there is no contradiction.
    //
    // Input: equations = [["a","b"],["b","c"]], values = [2.0,3.0], queries =
    // [["a","c"],["b","a"],["a","e"],["a","a"],["x","x"]]
    // Output: [6.00000, 0.50000, -1.00000, 1.00000, -1.00000]
    // Explanation :
    // Given : a / b = 2.0, b / c = 3.0
    // queries are : a / c = ? , b / a = ? , a / e = ? , a / a = ? , x / x = ?
    // return : [6.0, 0.5, -1.0, 1.0, -1.0]
    //---------------------------------------------------------------------------------------
    class Solution399
    {
    public:
        // Idea: This looks like a graph problem.
        struct Node
        {
            using EDGE_TYPE = pair<string, double>;
            // a / b = 3 means a -> b, distance = 3.
            // pair<destination node, distance> : pair<b, 3>.
            vector<EDGE_TYPE> edges;
            bool visited = false;

            Node() {}

            Node(Node&& aNode) noexcept
                : edges(move(aNode.edges))
            {
            }
        };

        vector<double> calcEquation
            (
            vector<vector<string>>& equations,
            vector<double>& values,
            vector<vector<string>>& queries
            )
        {
            unordered_map<string, Node> graphs;

            // Construct graphs.
            for (int i = 0; i < equations.size(); ++i)
            {
                const string& from = equations[i][0];
                const string& dest = equations[i][1];

                // Create the node when first time seeing it, or just add the new
                // link.
                if (graphs.count(from) == 0)
                {
                    Node fromNode;
                    fromNode.edges.push_back(Node::EDGE_TYPE(dest, values[i]));
                    graphs.insert(pair<string, Node>(from, move(fromNode)));
                }
                else
                {
                    graphs[from].edges.push_back(Node::EDGE_TYPE(dest, values[i]));
                }
                // Also create the reverse link
                // For example,  a -> b = 2, also create b -> a = 0.5.
                if (graphs.count(dest) == 0)
                {
                    Node destNode;
                    destNode.edges.push_back(Node::EDGE_TYPE(from, 1 / values[i]));
                    graphs.insert(pair<string, Node>(dest, move(destNode)));
                }
                else
                {
                    graphs[dest].edges.push_back(Node::EDGE_TYPE(from, 1 / values[i]));
                }
            }

            vector<double> result;
            for (const auto& query : queries)
            {
                const string& from = query[0];
                const string& dest = query[1];

                // If a node doesn't exist in the graph, return -1;
                if (graphs.count(from) == 0 || graphs.count(dest) == 0)
                {
                    result.push_back(-1);
                }
                else if (from == dest)
                {
                    // a / a = 1. Obviously.
                    result.push_back(1);
                }
                else
                {
                    // Do DFS to find the path to destination.
                    double distance = 1.0;
                    string currentNode = from;
                    bool ret = dfs(from, dest, graphs, distance);
                    result.push_back(ret ? distance : -1);
                }
            }

            return result;
        }

        bool dfs
            (
            const string& from,
            const string& dest, unordered_map<string, Node>& graphs,
            double& distance
            )
        {
            bool ret = false;
            for (const auto& edge : graphs[from].edges)
            {
                if (dest == edge.first)
                {
                    distance *= edge.second;
                    ret = true;
                    break;
                }
                // We built double-direction links. We must not go back, so we must check visited.
                else if(!graphs[edge.first].visited)
                {
                    distance *= edge.second;
                    graphs[from].visited = true;
                    ret = dfs(edge.first, dest, graphs, distance);
                    graphs[from].visited = false;
                    if (!ret)
                    {
                        distance /= edge.second;
                    }
                    else
                    {
                        break;
                    }
                }
            }

            return ret;
        }
    };


    //---------------------------------------------------------------------------------------
    // 332. Reconstruct Itinerary (Hard)
    // Topic: DFS, Graph, Eulerian circuit.
    //
    // This question requests us to use each edge exactly once.
    //
    // Real onsite interview question.
    //---------------------------------------------------------------------------------------
    class Solution332
    {
    public:

        // There are recursive and iterative ways to solve this.
        // In the recursive approach, we need to do DFS on a node, remove the edge we visited.
        // Stop when no edge left, insert the node reversely into result when retuning from
        // the recursive function.
        vector<string> findItinerary(vector<vector<string>>& tickets)
        {
            // multiset will sort the airports for use, so "ATL" is put before "SFO".
            unordered_map<string, multiset<string>> graphs;
            // Build adjacent list.
            for (const auto& ticket : tickets)
            {
                graphs[ticket[0]].insert(ticket[1]);
            }

            vector<string> result;
            dfs(graphs, "JFK", result);

            return vector<string>(result.rbegin(), result.rend());
        }

        void dfs(unordered_map<string, multiset<string>>& graphs, const string& start, vector<string>& result)
        {
            while(!graphs[start].empty())
            {
                string next = *graphs[start].begin();
                graphs[start].erase(graphs[start].begin());
                dfs(graphs, next, result);
            }

            result.push_back(start);
        }

        // In the iterative approach, the crucial part is how should use stack to
        // simulate the DFS.
        vector<string> findItinerary_iterative(vector<vector<string>>& tickets)
        {
            // multiset will sort the airports for use, so "ATL" is put before "SFO".
            unordered_map<string, multiset<string>> graphs;
            // Build adjacent list.
            for (const auto& ticket : tickets)
            {
                graphs[ticket[0]].insert(ticket[1]);
            }

            deque<string> result;
            stack<string> nodeStack;
            nodeStack.push("JFK");
            while (!nodeStack.empty())
            {
                const string cur = nodeStack.top();
                if (graphs[cur].empty())
                {
                    nodeStack.pop();
                    result.push_front(cur);
                }
                else
                {
                    const string next = *graphs[cur].begin();
                    // Delete the edge if we passed it.
                    graphs[cur].erase(graphs[cur].begin());
                    // It will be processed next. This is how we do the DFS.
                    nodeStack.push(next);
                }
            }

            return vector<string>(result.begin(), result.end());
        }
    };

    //---------------------------------------------------------------------------------------
    // 2115. Find All Possible Recipes from Given Supplies
    //---------------------------------------------------------------------------------------
    class Solution2115
    {
    public:

        struct RecipeNode
        {
            int index = 0;
            vector<string> outEdges;
            int inDegree = 0;
        };

        vector<string> findAllRecipes(vector<string>& recipes, vector<vector<string>>& ingredients, vector<string>& supplies)
        {
            const size_t len = recipes.size();
            unordered_map<string, RecipeNode> recipeGraph;

            for (int i = 0; i < len; ++i)
            {
                RecipeNode node;
                node.index = i;
                recipeGraph.insert(pair<string, RecipeNode>(recipes[i], node));
            }

            for (int i = 0; i < len; ++i)
            {
                const string& recipe = recipes[i];
                const vector<string>& ingredientSet = ingredients[i];

                for (int j = 0; j < ingredientSet.size(); ++j)
                {
                    const string& ingredient = ingredientSet[j];
                    if (recipeGraph.count(ingredient) > 0)
                    {
                        // If this ingredient is also a recipe and needed by the current recipe.
                        recipeGraph[ingredient].outEdges.push_back( recipe );
                        recipeGraph[recipe].inDegree++;
                    }
                }
            }

            queue<string> availableRecipe;
            for (const auto& recipe : recipeGraph)
            {
                if (recipe.second.inDegree == 0)
                {
                    availableRecipe.push(recipe.first);
                }
            }

            vector<string> result;
            while (!availableRecipe.empty())
            {
                const string recipe = availableRecipe.front();
                availableRecipe.pop();
                const int index = recipeGraph[recipe].index;
                if (canMake(ingredients[index], supplies))
                {
                    result.push_back(recipe);
                    supplies.push_back(recipe);
                }

                for (const auto& nextRecipe : recipeGraph[recipe].outEdges)
                {
                    recipeGraph[nextRecipe].inDegree--;
                    if (recipeGraph[nextRecipe].inDegree == 0)
                    {
                        availableRecipe.push(nextRecipe);
                    }
                }
            }

            return result;
        }

        bool canMake(const vector<string>& ingredients, const vector<string>& supplies)
        {
            for (const auto& ingredient : ingredients)
            {
                if (std::find(supplies.begin(), supplies.end(), ingredient) == supplies.end())
                {
                    return false;
                }
            }
            return true;
        }
    };

    //---------------------------------------------------------------------------------------
    // Got from Youtube: https://www.youtube.com/watch?v=qz9tKlF431k
    // (Mock Google Coding Interview)
    //
    // This is also asked in a Airbnb interview.
    //
    // Idea:
    // 1. Find strongly connected component.
    // 2. Compress into several component with different Id (or pick a node as representative
    //    node and use its Id?).
    // 3. Do topological sort on the components. Count the number of components that has
    //    0 in-degree.
    //
    // Input: airports = {"BGI", "CDG", "DEL", "DOH", "DSM", "EWR", "EYW", "HND", "ICN",
    // "JFK", "LGA", "LHR", "ORD", "SAN", "SFO", "SIN", "TLV", "BUD" }
    // routes: = { {"DSM", "ORD"}, {"ORD,"BGI"}, {"BGI", "LGA"}, {"SIN", "CDG"},
    // {"CDG", "SIN"}, {"CDG", "BUD"}, {"DEL", "DOH"}, {"DEL", "CDG"}, {"TLV", "DEL"},
    // {"EWR", "HND"}, {"HND", "ICN"}, {"HND", "JFK"}, {"ICN", "JFK"}, {"JFK", "LGA"},
    // {"EYW", "LHR"}, {"LHR", "SFO"}, {"SFO", "SAN"}, {"SFO", "DSM"}, {"SAN", "EYW"}
    // }
    // starting point: "LGA".
    //
    // Find out how many lines you will need to add to ensure you can travel all airports.
    // Ans: 3
    //---------------------------------------------------------------------------------------

    #if( 0 )
    typedef pair<int, int> pr;
    typedef unordered_map<int, unordered_set<int> > connectInfo;
    void fromDFS(int node, connectInfo from, vector<int>& order, unordered_set<int>& visited) {
        if (!visited.count(node)) {
            visited.insert(node);
            for (auto n : from[node])
                fromDFS(n, from, order, visited);
            order.insert(order.begin(), node);
        }
    }

    void toDFS(int node, int root, connectInfo to, unordered_map<int, int>& components) {
        if (!components.count(node)) {
            components[node] = root;
            for (auto n : to[node])
                toDFS(n, root, to, components);
        }
    }

    unordered_map<int, int> kosasrajus(connectInfo from, connectInfo to, const unordered_set<int>& nodes) {
        unordered_set<int> visited;
        unordered_map<int, int> components;
        vector<int> order;
        for (auto node : nodes)
            fromDFS(node, from, order, visited);
        for (auto node : order)
            toDFS(node, node, to, components);
        return components;
    }

    vector<int> least_nodes(const vector<pr>& edges) {
        connectInfo from, to;
        unordered_set<int> nodes;
        vector<int> ans;
        for (auto edge : edges) {
            from[edge.first].insert(edge.second);
            to[edge.second].insert(edge.first);
            nodes.insert(edge.first);
            nodes.insert(edge.second);
        }
        unordered_map<int, int> components = kosasrajus(from, to, nodes);
        unordered_set<int> fromComponents, toComponents;
        for (auto edge : edges) {
            fromComponents.insert(components[edge.first]);
            if (components[edge.first] != components[edge.second]) {
                toComponents.insert(components[edge.second]);
            }
        }
        for (auto node : fromComponents)
            if (!toComponents.count(node))
                ans.push_back(node);
        return ans;
    }
    #endif


    void TestGraph()
    {
        // 399. Evaluate Division
        vector<vector<string>> equations;
        equations = { {"a","b"} ,{"b","c"}, {"b", "d"}};
        vector<double> values = { 2.0, 3.0, 2.0};
        vector<vector<string>> queries = { {"a", "c"}, {"a", "d"} ,{"b", "a"}, {"a", "e"}, {"a", "a"}, {"x", "x"}};

        Solution399 sol399;
        auto resultVD = sol399.calcEquation(equations, values, queries);
        cout << "\n399. Evaluate Division:" << endl;
        LeetCodeUtil::PrintVector(resultVD);

        // 332. Reconstruct Itinerary (Hard)
        // Input: tickets = [["MUC", "LHR"], ["JFK", "MUC"], ["SFO", "SJC"], ["LHR", "SFO"]]
        // Output : ["JFK", "MUC", "LHR", "SFO", "SJC"]
        // Input: tickets = [["JFK","SFO"],["JFK","ATL"],["SFO","ATL"],["ATL","JFK"],["ATL","SFO"]]
        // Output: ["JFK", "ATL", "JFK", "SFO", "ATL", "SFO"]
        Solution332 sol332;
        //vector<vector<string>> inputVVS = { {"MUC", "LHR"},{"JFK", "MUC"},{"SFO", "SJC"},{"LHR", "SFO"} };
        vector<vector<string>> inputVVS = { {"JFK","SFO"},{"JFK","ATL"},{"SFO","ATL"},{"ATL","JFK"},{"ATL","SFO"} };
        auto resultVS = sol332.findItinerary_iterative(inputVVS);
        cout << "\n332. Reconstruct Itinerary:" << endl;
        LeetCodeUtil::PrintVector(resultVS);

        // 2115. Find All Possible Recipes from Given Supplies
        // Input: recipes = ["bread","sandwich"], ingredients = [["yeast","flour"],["bread","meat"]], supplies = ["yeast","flour","meat"]
        // Output: ["bread", "sandwich"]
        Solution2115 sol2115;
        vector<string> inputVS = { "bread","sandwich" };
        inputVVS = { {"yeast","flour"} ,{"bread","meat"} };
        vector<string> supplies = { "yeast","flour","meat" };
        resultVS = sol2115.findAllRecipes(inputVS, inputVVS, supplies);
        cout << "\n2115. Find All Possible Recipes from Given Supplies:" << endl;
        LeetCodeUtil::PrintVector(resultVS);
    }
}