<div align="center">
  <img width="125" alt="Sentinel Logo" src="./sentinel-min.png">
  <h3>Sentinel</h3>
</div>

<p align="center">UCI chess engine written in C++</p>

<hr />

<h3>Features</h3>

<h4>Search</h4>
<ul>
  <li>Aspiration windows</li>
  <li>Iterative deepening</li>
  <li>Killer moves</li>
  <li>History heuristic</li>
  <li>SEE</li>
  <li>Negamax alpha beta</li>
  <li>Internal iterative reductions</li>
  <li>Quiescence search</li>
  <li>Null move pruning</li>
  <li>Reverse futility pruning</li>
  <li>Late move pruning</li>
  <li>Late move reductions</li>
  <li>Transposition table</li>
  <ul>
    <li>TT cut offs</li>
  </ul>
</ul>

<h4>Eval</h4>
<p>All parameters were tuned using <a href="https://github.com/GediminasMasaitis/texel-tuner">Geda's Texel Tuner</a></p>
<ul>
  <li>Tapered eval</li>
  <li>Piece square tables</li>
  <li>King virtual mobility</li>
  <li>Simple piece mobility</li>
  <li>Rooks on open and semiopen files</li>
  <li>Isolated pawns </li>
  <li>Passed pawns</li>
  <li>Stacked pawns</li>
  <li>Two bishops bonus</li>
  <li>Castling bonus</li>
</ul>

<h4>Progress</h4>

  | Version     | Estimated   | CCRL 40/15  | CCRL Blitz 2 + 1
| ----------- | ----------- | ----------- | ---------------- |
| 1.0         | ?        | ?        | ?         |




<h4>Special thanks</h4>
<p>Many thanks to all Chads on the Chess Engine Programming Discord who helped me and taught me the basics of engine programming.</p>
