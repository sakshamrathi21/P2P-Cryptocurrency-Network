import os
import graphviz
import shutil

def read_blockchain(file_path):
    """Reads the blockchain data and returns the edges as a list of tuples (parent, child)."""
    edges = []
    with open(file_path, "r") as file:
        for line in file:
                parts = line.strip().split('\t')
                block, parent = map(int, parts[:2])
                edges.append((parent, block))
    return edges

def build_graph(edges):
    """Build a graph dictionary where each key is a parent node and the values are its children."""
    graph = {}
    for parent, child in edges:
        if parent not in graph:
            graph[parent] = []
        graph[parent].append(child)
    for parent in graph:
        graph[parent] = sorted(graph[parent]) # Sort children by block number
    return graph

def plot_blockchain_graphviz(edges, output_file, max_depth=200):
    """Generates a tree visualization of the blockchain up to the given max depth."""
    # Create a new directed graph
    dot = graphviz.Digraph(format='png', engine='dot')
    dot.attr(rankdir='LR')
    dot.attr(dpi='300')

    # Build the graph dictionary
    graph = build_graph(edges)

    visited = set()

    def add_nodes_and_edges(node, depth):
        """Recursive function to add nodes and edges up to a maximum depth."""
        if depth > max_depth:
            return  # Stop recursion when max depth is reached
        
        if node in visited:
            return  # Stop recursion if the node has already been visited
        
        # Add the node to the graph
        visited.add(node)
        dot.node(str(node))

        # Add edges to children if they exist
        if node in graph:
            for child in graph[node]:
                dot.node(str(child))
                dot.edge(str(node), str(child))
                add_nodes_and_edges(child, depth + 1)  # Recurse for the child

    # Start plotting from the root node
    if edges:
        start_node = -1
        add_nodes_and_edges(start_node, 1)

    # Render and save the output to a PNG file
    dot.render(output_file, cleanup=True)

# Remove any previous blockchain graphs
shutil.rmtree("blockchain_graphs", ignore_errors=True)
os.makedirs("blockchain_graphs")

input_directory = "blockchain_data"
output_directory = "blockchain_graphs"

for file_name in os.listdir(input_directory):
    file_path = os.path.join(input_directory, file_name)
    output_file = os.path.join(output_directory, file_name.split(".")[0])

    edges = read_blockchain(file_path)
    plot_blockchain_graphviz(edges, output_file)