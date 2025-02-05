## Todo

*functionality -> elo gain*

### Eval

* No more HCE - Sentinel 2.0 has NNUE *
* Data generated via self-play.*

- [X] HCE => NNUE -> ?

Net history:
- Simple nets - 16 neuron, 24 neuron.
- Aegis net - 64 neuron net trained from HCE, 16 neuron, 24 neuron selfplay data.
- Delta net - 64 neuron net trained from Aegis selfplay data
- Singularity - 128 neuron net trained from Aegis, HCE, [16 && 24] neuron net selfplay data.
  - 5000 soft-nodes limit.

- Singularity_v2 - 128 neuron net trained from Singularity selfplay data. [~120 mil positions]
  - 5000 soft-nodes limit

- Void - 224 neuron net trained from Singularity selfplay data. [~230 mil. positions]
  - 5000 soft-nodes limit

When Void is trained Tune search parameters for new eval.
    -> Release of 2.0

3.0 plans:
- [ ] Internal speedups.
- [ ] The most of Search TODOs.
- [X] SIMD [for net performance]
- [ ] Bigger nets [512,1024,2048] + learn about "bucketing"
    - [ ] More layers [??]

------

### Search
- [ ] Better move ordering -> ?
    - [X] Counter moves -> ?
    - [ ] Better history - max history values
    - [ ] Capture history -> ?
    - [ ] Continuation history -> ?
        - [ ] 1 ply -> ?
        - [ ] 2 ply -> ?
- [ ] Futility pruning -> ?
- [ ] Late move pruning -> ?
- [ ] Check extension -> ?
- [ ] Q Search TT -> ?
- [ ] Singular extensions -> ?
    - [ ] Multicut -> ?

------

### Enchantments
- [x] PV
- [ ] TT Prefetching -> ? 
  - no elo gain, yet
- [ ] Multithread
    - [ ] Lazy SMP
- [ ] Legal movegen -> ?
