#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <list>
#include <algorithm>

using namespace std;

struct Node {
    int id;
    vector<int> inputs;
    vector<int> outputs;
    bool isPrimary = false; 
    bool used = false; 

    Node(const int node, const vector<int>& inputIds = {}, bool primary = false)
        : id(node), inputs(inputIds), isPrimary(primary) {}
};

class BooleanNetwork {
public:
    unordered_map<int, Node*> nodes;
    unordered_map<int, int> outputMap;

    void addNode(const Node& node) {
        int nodeId = node.id;
        if (nodes.find(nodeId) == nodes.end()) {
            nodes[nodeId] = new Node(node.id, node.inputs, node.isPrimary);
        }
        nodes[nodeId]->inputs = node.inputs;
        nodes[nodeId]->isPrimary = node.isPrimary;

        for (int inputId : node.inputs) {
            if (nodes.find(inputId) == nodes.end()) {
                nodes[inputId] = new Node({ inputId });
            }
            nodes[inputId]->outputs.push_back(nodeId);
        }
    }

    vector<int> topologicalSort() {
        unordered_map<int, int> inDegree;
        for (const auto& pair : nodes) {
            inDegree[pair.first] = 0;
        }

        for (const auto& pair : nodes) {
            for (int output : pair.second->outputs) {
                inDegree[output]++;
            }
        }

        list<int> zeroInDegreeQueue;
        for (const auto& pair : inDegree) {
            if (pair.second == 0) {
                zeroInDegreeQueue.push_back(pair.first);
            }
        }

        vector<int> sortedNodes;
        while (!zeroInDegreeQueue.empty()) {
            int node = zeroInDegreeQueue.front();
            zeroInDegreeQueue.pop_front();
            sortedNodes.push_back(node);

            for (int output : nodes[node]->outputs) {
                if (--inDegree[output] == 0) {
                    zeroInDegreeQueue.push_back(output);
                }
            }
        }

        if (sortedNodes.size() != nodes.size()) {
            cerr << "Error: The graph has a cycle, topological sort is not possible." << endl;
        }

        return sortedNodes;
    }

    void synthesizeLUTs(int K) {
        vector<int> sortedNodes = topologicalSort();
        int newLUTId = nodes.size() + 1;
        int oldLUTsize = nodes.size();
        list<int> extendedSortedNodes(sortedNodes.begin(), sortedNodes.end());

        for (auto it = extendedSortedNodes.begin(); it != extendedSortedNodes.end(); ++it) {
            int nodeId = *it;
            Node* node = nodes[nodeId];

            if (node->isPrimary || node->used) continue;

            vector<int> currentLUT = { nodeId };
            int currentInputCount = node->inputs.size();
            vector<int> mergedLUTs;

            for (int inputId : node->inputs) {
                Node* inputNode = nodes[inputId];

                bool canMerge = true;
                for (int outputId : inputNode->outputs) {
                    if (outputId != nodeId) {
                        canMerge = false;
                        break;
                    }
                }

                if (!canMerge) continue;

                if (inputNode->isPrimary) {
                    continue;
                }
                else if (!inputNode->used) {
                    int inputSize = inputNode->inputs.size();
                    for (int input : inputNode->inputs) {
                        if (find(node->inputs.begin(), node->inputs.end(), input) != node->inputs.end()) {
                            inputSize--;
                        }
                    }
                    currentInputCount = currentInputCount - 1 + inputSize;
                    if (currentInputCount <= K) {
                        currentLUT.push_back(inputId);
                        if (inputNode->id > oldLUTsize) {
                            mergedLUTs.push_back(inputId);
                        }
                        inputNode->used = true;
                    }
                    else {
                        continue;
                    }
                }
            }

            if ((currentLUT.size() > 0 && nodeId <= oldLUTsize) || (currentLUT.size() > 1 && nodeId > oldLUTsize)) {
                node->used = true;
                vector<int> newnode_in;
                for (int old : currentLUT) {
                    for (int inpu : nodes[old]->inputs) {
                        if (find(newnode_in.begin(), newnode_in.end(), inpu) == newnode_in.end() &&
                            find(currentLUT.begin(), currentLUT.end(), inpu) == currentLUT.end()) {
                            newnode_in.push_back(inpu);
                        }
                    }
                }

                Node* newLUTNode = new Node(newLUTId, newnode_in, false);
                newLUTNode->outputs = node->outputs;
                nodes[newLUTId] = newLUTNode;

                for (int oldNodeId : currentLUT) {
                    for (int outputId : nodes[oldNodeId]->outputs) {
                        Node* outputNode = nodes[outputId];
                        replace(outputNode->inputs.begin(), outputNode->inputs.end(), oldNodeId, newLUTId);
                    }
                }

                for (int oldNodeId : currentLUT) {
                    for (int inputId : nodes[oldNodeId]->inputs) {
                        Node* inputNode = nodes[inputId];
                        replace(inputNode->outputs.begin(), inputNode->outputs.end(), oldNodeId, newLUTId);
                    }
                }

                auto nextIt = next(it);
                extendedSortedNodes.insert(nextIt, newLUTId);
                outputMap[newLUTId] = nodeId;
                newLUTId++;
            }
        }
    }

bool readFromFile(const string& inputFile) {
    ifstream inFile(inputFile);
    if (!inFile.is_open()) {
        cerr << "Failed to open input file: " << inputFile << endl;
        return false;
    }

    string line;
    int nodeId, numNodes, numPIs, numPOs;

    // Read header
    getline(inFile, line);
    istringstream headerStream(line);
    string name;
    headerStream >> name >> numNodes >> numPIs >> numPOs;

    // Read Primary Inputs
    for (int i = 0; i < numPIs; i++) {
        inFile >> nodeId;
        addNode(Node(nodeId, {}, true));
    }

    // Read Primary Outputs
    vector<int> primaryOutputs;
    for (int i = 0; i < numPOs; i++) {
        inFile >> nodeId;
        primaryOutputs.push_back(nodeId);
    }

    // Read remaining nodes
    while (getline(inFile, line)) {
        istringstream nodeStream(line);
        nodeStream >> nodeId;
        int inputId;
        vector<int> inputs;
        
        while (nodeStream >> inputId) {
            inputs.push_back(inputId);
        }
        addNode(Node(nodeId, inputs, false));
    }

    inFile.close();
    return true;
}

    int getOriginalNode(int nodeId) {
        while (outputMap.find(nodeId) != outputMap.end()) {
            nodeId = outputMap[nodeId];
        }
        return nodeId;
    }

    void printNetwork(const string& outputFile) {
        ofstream outFile(outputFile);
        if (!outFile.is_open()) {
            cerr << "Failed to open output file: " << outputFile << endl;
            return;
        }

        for (const auto& pair : nodes) {
            Node* node = pair.second;

            if (!node->used && outputMap.find(node->id) != outputMap.end()) {
                int outputNodeId = getOriginalNode(node->id);
                const vector<int>& inputs = node->inputs;

                outFile << outputNodeId;
                for (int inputId : inputs) {
                    int inputNodeId = getOriginalNode(inputId);
                    outFile << " " << inputNodeId;
                }
                outFile << endl;
            }
        }

        outFile.close();
    }


    ~BooleanNetwork() {
        for (auto& pair : nodes) {
            delete pair.second;
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file> <K>" << endl;
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = argv[2];
    int K = stoi(argv[3]);

    BooleanNetwork network;

    if (!network.readFromFile(inputFile)) {
        return 1;
    }
	
    network.synthesizeLUTs(K);
    network.printNetwork(outputFile);

    return 0;
}