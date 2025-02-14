#ifndef SENTINEL_TUNABLES_H
#define SENTINEL_TUNABLES_H

#include "tunable.h"

// (name, min, max, def)

static inline const Tunable NMP_DEPTH {"NMP_DEPTH", 1, 6, 3};
static inline const Tunable NMP_REDUCTION_CONSTANT {"NMP_REDUCTION_CONSTANT", 1, 6, 3};
static inline const Tunable NMP_REDUCTION_DIV_CONSTANT {"NMP_REDUCTION_DIV_CONSTANT", 1, 6, 3};

static inline const Tunable LMR_DEPTH {"LMR_DEPTH", 1, 5, 2};
static inline const Tunable LMR_MOVE_COUNT_ADD {"LMR_MOVE_COUNT_ADD", 1, 5, 3};

static inline const Tunable ASPIRATION_DELTA_START {"ASPIRATION_DELTA_START", 4, 20, 15};
static inline const Tunable ASPIRATION_MAX_DELTA_SIZE {"ASPIRATION_MAX_DELTA_SIZE", 1, 1000, 481};

static inline const Tunable IIR_DEPTH {"IIR_DEPTH", 1, 10, 5};

static inline const Tunable IMPROVING_CONSTANT {"IMPROVING_CONSTANT", 0, 100, 41};

static inline const Tunable RFP_DEPTH {"RFP_DEPTH", 1, 10, 8};
static inline const Tunable RFP_CONSTANT {"RFP_CONSTANT", 0, 200, 92};

static inline const Tunable SI_DEPTH {"SI_DEPTH", 1, 10, 8};
static inline const Tunable SI_DEPTH_MUL {"SI_DEPTH_MUL", 1, 5, 1};
static inline const Tunable SI_DEPTH_TT_ADD {"SI_DEPTH_TT_ADD", 1, 5, 3};

static inline const Tunable LMP_DEPTH {"LMP_DEPTH", 1, 10, 5};

static inline const Tunable FP_DEPTH {"FP_DEPTH", 1, 10, 7};
static inline const Tunable FP_CONSTANT {"FP_CONSTANT", 1, 200, 140};

static inline const Tunable SEE_QUIET_DEPTH {"SEE_QUIET_DEPTH", 1, 10, 7};
static inline const Tunable SEE_QUIET_THRESHOLD {"SEE_QUIET_THRESHOLD", 1, 125, 80};

static inline const Tunable SEE_CAP_DEPTH {"SEE_CAP_DEPTH", 1, 10, 7};
static inline const Tunable SEE_CAP_THRESHOLD {"SEE_CAP_THRESHOLD", 1, 125, 40};


#endif //SENTINEL_TUNABLES_H
