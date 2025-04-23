# Minimize-of-the-number-of-K-LUTs-used-for-Boolean-function
We can represent a Boolean network as a directed acyclic graph in which each node
represents a logic gate, and in which a directed edge (𝑖,𝑗) exists if the output of gate
𝑖 is an input of gate 𝑗. A primary input (PI) node has no incoming edge, and a primary
output (PO) node has no outgoing edge. We use 𝑖𝑛𝑝𝑢𝑡𝑠(𝑢) to denote the set of nodes
that supply inputs to gate 𝑢. Given a subgraph 𝐻 of a Boolean network, 𝑖𝑛𝑝𝑢𝑡𝑠(𝐻)
denotes the set of distinct nodes outside 𝐻 which supply inputs to the gates in 𝐻. For
a node 𝑣 in the network, a K-feasible cone at 𝑣, denoted 𝐶𝑣
, is a subgraph consisting
of 𝑣 and its predecessors such that |𝑖𝑛𝑝𝑢𝑡𝑠(𝐶𝑣)| ≤ 𝐾 and any path connecting a
node in 𝐶𝑣 and 𝑣 lies entirely in 𝐶𝑣
. A Boolean network is K-bounded if
𝑖𝑛𝑝𝑢𝑡𝑠(𝑣) ≤ 𝐾 for each node 𝑣. We assume that each programmable logic block in
an FPGA is a K-input one-output lookup-table (K-LUT) that can implement any Kinput Boolean function. Thus, each K-LUT can implement any K-feasible cone of a
Boolean network. The technology mapping problem is to cover a given Boolean
network with K-feasible cones.
