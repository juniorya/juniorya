#include "planner/lookahead/lookahead.h"

q16_16 lookahead_evaluate(const lookahead_segment *segments, size_t count)
{
    if ((segments == NULL) || (count == 0U))
    {
        return q16_from_int(0);
    }
    q16_16 feed = segments[0].feed;
    for (size_t i = 0U; i < count; ++i)
    {
        q16_16 curvature_factor = q16_from_int(1);
        if (segments[i].curvature > q16_from_int(1))
        {
            curvature_factor = q16_div(q16_from_int(1), segments[i].curvature);
        }
        q16_16 candidate = q16_mul(segments[i].feed, curvature_factor);
        if (candidate < feed)
        {
            feed = candidate;
        }
    }
    return feed;
}
