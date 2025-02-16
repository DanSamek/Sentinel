<div align="center">
  <img width="125" alt="Sentinel Logo" src="./sentinel-min.png">
  <h3>Sentinel</h3>
</div>

<p align="center">UCI chess engine written in C++</p>

<hr />

<h3>Build</h3>

```  
cmake -DCMAKE_BUILD_TYPE=Release
or for AVX:
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_AVX=ON
or for SSE:
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_SSE=ON

cmake --build .
or
make
```

<h4> NNUE </h4>
<p>Sentinel 2.0 uses a neural network to evaluate positions.</p>
<p>The current NNUE architecture is (768->128)x2->1 trained on self-play data.</p>
<i>I currently don't have the resources to train larger networks, but in the future I would like to train at least (768->512)x2->1.</i>

<h4>Search</h4>
<p>Sentinel uses many search heuristics that were inspired by top chess engines or found on a chess programming wiki.</p>

<h4>Progress</h4>

| Version     | Estimated    | CCRL 40/15  | CCRL Blitz 2 + 1
| ----------- | -----------  | ----------- | ---------------- |
| 1.0         | 2.3-2.5k     | 2425        | ?                |
| 2.0         | 2.7-3k       | ?           | ?                |

<h4>Special thanks</h4>
<p>Many thanks to all Chads on the Chess Engine Programming Discord who helped me and taught me the basics of engine programming.</p>
<p>Bullet - a NNUE trainer that has been used to train neural networks.</p>

