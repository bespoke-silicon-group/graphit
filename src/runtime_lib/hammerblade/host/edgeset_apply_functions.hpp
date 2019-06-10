#pragma once


#include "vertexsubset.h"
#include "infra_gapbs/builder.h"
#include "infra_gapbs/benchmark.h"
#include "infra_gapbs/bitmap.h"
#include "infra_gapbs/command_line.h"
#include "infra_gapbs/graph.h"
#include "infra_gapbs/platform_atomics.h"
#include "infra_gapbs/pvector.h"

#include "infra_ligra/ligra/ligra.h"
#include "infra_ligra/ligra/utils.h"


template <const char *TO_FUNCTION>
VertexSubset<NodeID> *edgeset_apply_hammerblade(Graph &g)
{
}
