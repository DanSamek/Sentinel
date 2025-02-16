## Todo

*functionality -> elo gain*

### Eval

* No more HCE - Sentinel 2.0 has NNUE *
* Data generated via self-play.*

- [X] HCE => NNUE -> ~200-300 ELO

Net history:
- Simple nets - 16 neuron, 24 neuron.
- Aegis net - 64 neuron net trained from HCE, 16 neuron, 24 neuron selfplay data.
- Delta net - 64 neuron net trained from Aegis selfplay data
- Singularity - 128 neuron net trained from Aegis, HCE, [16 && 24] neuron net selfplay data.
  - 5000 soft-nodes limit.
- Singularity_v2 - 128 neuron net trained from Singularity selfplay data. [~120 mil positions]
  - 5000 soft-nodes limit

3.0 plans:
- [ ] Internal speedups.
- [ ] The most of Search TODOs.
- [X] SIMD [for net performance]
- [ ] Bigger nets [256, 512, 1024, 2048] + learn about "bucketing"
    - [ ] More layers [??]

------

### Search
- [ ] Better move ordering
    - [X] Counter moves
    - [X] Better history - max history values
    - [ ] Capture history
    - [ ] Continuation history
        - [ ] 1 ply
        - [ ] 2 ply
- [X] Futility pruning
- [X] Late move pruning
- [X] Check extension
- [X] Q Search TT
- [X] Singular extensions
    - [X] Multicut

------

### Enchantments
- [X] PV
- [X] TT Prefetching
  - no elo gain, yet
- [ ] Multithread
    - [ ] Lazy SMP
- [ ] Legal movegen
