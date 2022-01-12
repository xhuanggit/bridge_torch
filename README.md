# Bridge and torch problem

## Description

Multiple hikers cross a series of narrow bridges at various speeds. Each bridge can only hold at most 2 hikers at one time. When 2 hikers cross the bridge together, they must move at the slower hiker's speed. There is 1 torch that has to be used when crossing the bridge.

More hikers can join the team at farther bridges. One needs to find the minimal total crossing time.

The input is given in a yaml file including hiker speeds, starting bridges and bridge lengths.

## Strategy

Since the torch needs to be used, when 2 hikers cross, 1 hiker has to come back to pass the torch. Consider a simple case with 4 hikers a, b, c and d crossing one bridge, where the hiker speeds are sorted in descending order. There are in general two ways to cross:

Arrangement 1: ab, a, ac, a, ad; where hiker a always carries back the torch;\
Total time: Forward: T(b)+T(c)+T(d), Backward: T(a)\*2;

Arrangement 2: ab, a, cd, b, ab; where hikers a and b alternatively carry back the torch;\
Total time: Forward: T(b)\*2+T(d), Backward: T(a)+T(b);

To compare the two ways of crossing is actually to compare T(a)+T(c) and T(b)\*2.

Arrangement 1 aims at minimizing backward crossing time, while Arrangement 2 aims at minimizing forward crossing time by pairing the slowest two hikers together. Also in Arrangement 2 the fastest two hikers carry the torch back. Other ways of crossing cannot win over either arrangement.

For n hikers and one bridge, where all hikers speeds are sorted in descending order, same comparison can be made between the two arrangements for hikers 0, 1 (fastest two) and n-2, n-1 (slowest two). After the slowest two hikers cross, the state of the problem returns to n-2 hikers. Therefore one can solve the problem by induction. The base cases are either 2 or 3 hikers which are easy to analyze. A formula can also be derived.

For multiple bridges, one can include the new hiker speeds and sort together with previous ones. In implementation one can also save the speed inverse and sort increasingly.

## Implementation

This program uses yaml-cpp to process yaml input files which are located in the folder input/. The repository can be found at https://github.com/jbeder/yaml-cpp. The header and library files are copied to the folder yaml-cpp/.

To compile, in root directory one can use e.g. g++:

g++ -o bridge_torch_prog src/bridge_torch_main.cpp -I. -lyaml-cpp -Lyaml-cpp

The default input file is input/input.yaml. One additional argument can be given as the suffix of the filename, e.g. xhyb for input/inputxhyb.yaml. Several test inputs are also given.

Test results:
<pre>
Filename suffix	Time (minutes)
None			245
1h1b			4
2h1b			2
3h1b			7
4h1b1			15
4h1b2			18
5h1b			20
5h2b			39
</pre>