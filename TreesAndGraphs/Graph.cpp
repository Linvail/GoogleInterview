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

        struct Node
        {
            bool isRecipe = false;
            vector<string> outEdges;
            int inDegree = 0;
        };

        vector<string> findAllRecipes(vector<string>& recipes, vector<vector<string>>& ingredients, vector<string>& supplies)
        {
            const size_t len = recipes.size();
            // All recipes and ingredients are considered as nodes.
            unordered_map<string, Node> graph;
            //unordered_set<string> supplySet(supplies.begin(), supplies.end());

            for (int i = 0; i < len; ++i)
            {
                Node node;
                node.isRecipe = true;
                graph.insert(pair<string, Node>(recipes[i], node));
            }

            // Build adjacent list.
            for (int i = 0; i < len; ++i)
            {
                const vector<string>& ingredientSet = ingredients[i];
                for (int j = 0; j < ingredientSet.size(); ++j)
                {
                    const string& ingredient = ingredientSet[j];
                    graph[ingredient].outEdges.push_back(recipes[i]);
                    graph[recipes[i]].inDegree++;
                }
            }

            queue<string> availableQueue;
            for (const auto& node : graph)
            {
                // In the beginning, only ingredient could have 0 inDegree.
                // We need to check if it is in supplies.
                if (node.second.inDegree == 0 && find(supplies.begin(), supplies.end(), node.first) != supplies.end())
                {
                    availableQueue.push(node.first);
                }
            }

            vector<string> result;
            while (!availableQueue.empty())
            {
                const string node = availableQueue.front();
                availableQueue.pop();

                if (graph[node].isRecipe)
                {
                    // If this is a recipe, don't add ingredient.
                    result.push_back(node);
                }

                for (const auto& nextNode : graph[node].outEdges)
                {
                    graph[nextNode].inDegree--;
                    if (graph[nextNode].inDegree == 0)
                    {
                        availableQueue.push(nextNode);
                    }
                }
            }

            return result;
        }
    };

    //---------------------------------------------------------------------------------------
    // Inspired by Youtube video: https://www.youtube.com/watch?v=qz9tKlF431k
    // (Mock Google Coding Interview)
    //
    // It's said that the similar question was asked in a Airbnb interview.
    //
    // Idea:
    // 1. Find strongly connected component.
    // 2. Compress into several component with different Id (or pick a node as representative
    //    node and use its Id?).
    // 3. Do topological sort on the components. Count the number of components that has
    //    0 in-degree.
    //
    // Input:
    // airports = {"BGI", "CDG", "DEL", "DOH", "DSM", "EWR", "EYW", "HND", "ICN",
    // "JFK", "LGA", "LHR", "ORD", "SAN", "SFO", "SIN", "TLV", "BUD" };
    // routes =
    // {
    // {"DSM", "ORD"}, {"ORD", "BGI"}, {"BGI", "LGA"}, {"SIN", "CDG"},
    // {"CDG", "SIN"}, {"CDG", "BUD"}, {"DEL", "DOH"}, {"DEL", "CDG"}, {"TLV", "DEL"},
    // {"EWR", "HND"}, {"HND", "ICN"}, {"HND", "JFK"}, {"ICN", "JFK"}, {"JFK", "LGA"},
    // {"EYW", "LHR"}, {"LHR", "SFO"}, {"SFO", "SAN"}, {"SFO", "DSM"}, {"SAN", "EYW"}
    // };
    // startingAirport = "LGA";
    //
    // Find out how many lines you will need to add to ensure you can travel all airports.
    // Ans: 3
    // The following code will also give what lines they are:
    // Add lines from LGA to [LHR,TLV,EWR].
    //---------------------------------------------------------------------------------------

    using RouteType = pair<string, string>;
    using RoutesMapType = unordered_map<string, unordered_set<string>>;

    void towardDfs(const string& node, const RoutesMapType& graph, deque<string>& order, unordered_set<string>& visited)
    {
        if (!visited.count(node))
        {
            visited.insert(node);
            for (const auto& n : graph.at(node))
            {
                towardDfs(n, graph, order, visited);
            }
            order.push_front(node);
        }
    }

    void reversedDfs(const string& node, const string& root, const RoutesMapType& graph, unordered_map<string, string>& components)
    {
        if (!components.count(node))
        {
            components[node] = root;
            for (const auto& n : graph.at(node))
            {
                reversedDfs(n, root, graph, components);
            }
        }
    }

    unordered_map<string, string> findAllSsc(const RoutesMapType& graph, const RoutesMapType& reversedGraph, const unordered_set<string>& nodes)
    {
        // Helper to detect if a node has been visited during DFS.
        unordered_set<string> visited;
        // Adjacent list to represent the component.
        // <node's name, root's name of the component that this node belongs to>
        unordered_map<string, string> components;
        // Store the visited node in chronologically order.
        // First visited node is put in the end.
        deque<string> order;

        for (const auto& node : nodes)
        {
            towardDfs(node, graph, order, visited);
        }

        for (const auto& node : order)
        {
            // Call DFS on this node, every node visited by this DFS call will be added into components.
            // The 2nd 'node' parameter means this 'node' is the representative node of that SSC.
            reversedDfs(node, node, reversedGraph, components);
        }

        return components;
    }

    vector<string> makeNewRoutes(const vector<string>& airports, const vector<RouteType>& routes, const string& startingAirport)
    {
        RoutesMapType graph;
        RoutesMapType reversedGraph;
        unordered_set<string> nodes;

        // Scan the routes, build adjacent lists in graph/reversedGraph.
        // Also push every airports we ever meet into nodes.
        for (const auto& edge : routes)
        {
            graph[edge.first].insert(edge.second);
            // Some airports may has 0 outbound, we still need to create it in the graph.
            graph.emplace(edge.second, unordered_set<string>());
            reversedGraph[edge.second].insert(edge.first);
            // Some airports may has 0 inbound, we still need to create it in the reversedGraph.
            reversedGraph.emplace(edge.first, unordered_set<string>());
            nodes.insert(edge.first);
            nodes.insert(edge.second);
        }

        // This components stores the component's represent node's name of every node.
        // For example, ("CDG", "CDG"), ("SIN", "CDG"), means SIN and CDG belong
        // to the same component - CDG.
        unordered_map<string, string> components = findAllSsc(graph, reversedGraph, nodes);

        // If a SSC's in-degree is 0, we are unable to reach it.
        // That means we need to a line to connect to it.
        // Scan edges and establish the connection relationship between SSC.
        unordered_map<string, int> inDegreeSsc;
        for (const auto& edge : routes)
        {
            const string& sscOfStart = components[edge.first];
            const string& sscOfdest = components[edge.second];
            // Note that we are dealing with SSC, not airport.
            // If two airports are in the same SSC, skip.
            if (sscOfStart != sscOfdest)
            {
                inDegreeSsc[sscOfdest]++;
            }
            // emplace() won't take place if the key already exists.
            // Ensure every SSC would exist in the inDegreeSsc.
            inDegreeSsc.emplace(sscOfStart, 0);
        }

        // Collect all nodes whose in-degree is 0.
        vector<string> result;
        for (const auto& pr : inDegreeSsc)
        {
            if (pr.second == 0)
            {
                result.push_back(pr.first);
            }
        }

        return result;
    }


    //---------------------------------------------------------------------------------------
    // Test function
    //---------------------------------------------------------------------------------------
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
        // Input: recipes = {"bread","sandwich"}, ingredients = {{"yeast","flour"},{"bread","meat"}}, supplies = {"yeast","flour","meat"}
        // Output: {"bread", "sandwich"}
        // Input: recipes = {"bread"}, ingredients = {{"yeast","flour"}}, supplies = {"yeast"}
        // Output: []
        Solution2115 sol2115;
        vector<string> inputVS = { "bread" };
        inputVVS = { {"yeast","flour"} };
        vector<string> supplies = { "yeast" };
        resultVS = sol2115.findAllRecipes(inputVS, inputVVS, supplies);
        cout << "\n2115. Find All Possible Recipes from Given Supplies:" << endl;
        LeetCodeUtil::PrintVector(resultVS);

        // We have 18 airports.
        vector<string> airports =
            { "BGI", "CDG", "DEL", "DOH", "DSM", "EWR", "EYW", "HND", "ICN",
              "JFK", "LGA", "LHR", "ORD", "SAN", "SFO", "SIN", "TLV", "BUD" };
        vector<pair<string, string>> routes =
           { {"DSM", "ORD"}, {"ORD", "BGI"}, {"BGI", "LGA"}, {"SIN", "CDG"},
             {"CDG", "SIN"}, {"CDG", "BUD"}, {"DEL", "DOH"}, {"DEL", "CDG"}, {"TLV", "DEL"},
             {"EWR", "HND"}, {"HND", "ICN"}, {"HND", "JFK"}, {"ICN", "JFK"}, {"JFK", "LGA"},
             {"EYW", "LHR"}, {"LHR", "SFO"}, {"SFO", "SAN"}, {"SFO", "DSM"}, {"SAN", "EYW"} };
        string startingAirport = "LGA";

        resultVS = makeNewRoutes(airports, routes, startingAirport);
        cout << "\nMake new routes for airlines:" << endl;
        LeetCodeUtil::PrintVector(resultVS);
        cout << "\nExpected: [LHR,TLV,EWR]\n";
    }
}