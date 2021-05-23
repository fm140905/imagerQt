#ifndef RECONSTRUCTION_H
#define RECONSTRUCTION_H


#include "TH2D.h"
#include "setup.h"

/**
 * @brief Project cones onto the sphereical surface and update the image.
 *        Implementation based on
 *        https://www.nature.com/articles/s41598-020-58857-z
 *
 * @param config Input. Global settings.
 * @param histo Input and output. Image to be updated.
 * @param counts Input and output. Number of cones (events).
 * @param first Input. First iterator to the vector of cones.
 * @param last Input. Last iterator to the vector of cones.
 * @return 0
 */
int addCones(const Setup* config, TH2D* histo, ULong64_t& counts,
             std::vector<Cone>::const_iterator first,
             std::vector<Cone>::const_iterator last);

/**
 * @brief Project cones onto the sphereical surface and update the image.
 *        Imeplemention based on
 *        https://www.overleaf.com/read/hjdvrcrjcvpx
 *
 * @param config Input. Global settings.
 * @param histo Input and output. Image to be updated.
 * @param counts Input and output. Number of cones (events).
 * @param first Input. First iterator of the vector of cones.
 * @param last Input. Last iterator of the vector of cones.
 * @return 0
 */
int addConesNormalized(const Setup* config, TH2D* histo, ULong64_t& counts,
             std::vector<Cone>::const_iterator first,
             std::vector<Cone>::const_iterator last);

#endif // RECONSTRUCTION_H
