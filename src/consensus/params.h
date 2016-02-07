// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CONSENSUS_PARAMS_H
#define BITCOIN_CONSENSUS_PARAMS_H

#include "amount.h"
#include "uint256.h"

#include <memory>

/* Handle fork heights.  The function checks whether a fork is in effect
   at the given height -- and may use different heights for testnet
   and mainnet, or for a "testing mode".  */
enum Fork
{

  /* Poison disaster, increased general cost 1 HUC -> 10 HUC, just general
     as initial character.  */
  FORK_POISON,

  /* Maximum carrying-capacity introduced, removed spawn death,
     new-style name registration, stricter rule checks for transaction
     version and auxpow (in parallel to Namecoin).  */
  FORK_CARRYINGCAP,

  /* Update parameters (general 10 HUC -> 200 HUC, carrying capacity increased
     to 2000 HUC, heart spawn rate reduced to 1/500, general explosion
     radius only 1).  */
  FORK_LESSHEARTS,

  /* Implement "life steal".  This adds a game fee for destructs (5 HUC),
     completely disables hearts and removes all "hearted" hunters.  It also
     randomises spawn and banking locations.  */
  FORK_LIFESTEAL,

};

/** Dual-algo PoW algorithms.  */
enum PowAlgo
{
  ALGO_SHA256D = 0,
  ALGO_SCRYPT,
  NUM_ALGOS
};

namespace Consensus {

/**
 * Interface for classes that define consensus behaviour in more
 * complex ways than just by a set of constants.
 */
class ConsensusRules
{
public:

    /* Check whether a given fork is in effect at the height.  */
    virtual bool ForkInEffect(Fork type, unsigned nHeight) const = 0;

    /* Check whether the height is *exactly* when the fork starts to take
       effect.  This is used sometimes to trigger special events.  */
    inline bool
    IsForkHeight (Fork type, unsigned nHeight) const
    {
      if (nHeight == 0)
        return false;

      return ForkInEffect (type, nHeight) && !ForkInEffect (type, nHeight - 1);
    }

};

class MainNetConsensus : public ConsensusRules
{
public:

    bool ForkInEffect(Fork type, unsigned nHeight) const
    {
        switch (type)
        {
            case FORK_POISON:
                return nHeight >= 255000;
            case FORK_CARRYINGCAP:
                return nHeight >= 500000;
            case FORK_LESSHEARTS:
                return nHeight >= 590000;
            case FORK_LIFESTEAL:
                return nHeight >= 795000;
            default:
                assert (false);
        }
    }

};

class TestNetConsensus : public MainNetConsensus
{
public:

    bool ForkInEffect(Fork type, unsigned nHeight) const
    {
        switch (type)
        {
            case FORK_POISON:
                return nHeight >= 190000;
            case FORK_CARRYINGCAP:
                return nHeight >= 200000;
            case FORK_LESSHEARTS:
                return nHeight >= 240000;
            case FORK_LIFESTEAL:
                return nHeight >= 301000;
            default:
                assert (false);
        }
    }

};

class RegTestConsensus : public TestNetConsensus
{
public:

};

/**
 * Parameters that influence chain consensus.
 */
struct Params {
    uint256 hashGenesisBlock;
    int nSubsidyHalvingInterval;
    /** Used to check majorities for block version upgrade */
    int nMajorityEnforceBlockUpgrade;
    int nMajorityRejectBlockOutdated;
    int nMajorityWindow;
    /** Block height and hash at which BIP34 becomes active */
    int BIP34Height;
    uint256 BIP34Hash;
    /** Proof of work parameters */
    uint256 powLimit[NUM_ALGOS];
    bool fPowNoRetargeting;
    int64_t nPowTargetSpacing;
    int64_t nPowTargetTimespan;
    int64_t DifficultyAdjustmentInterval() const { return nPowTargetTimespan / nPowTargetSpacing; }
    /** Auxpow parameters */
    int32_t nAuxpowChainId[NUM_ALGOS];
    bool fStrictChainId;

    /** Consensus rule interface.  */
    std::auto_ptr<ConsensusRules> rules;

    /**
     * Check whether or not to allow legacy blocks at the given height.
     * @param nHeight Height of the block to check.
     * @return True if it is allowed to have a legacy version.
     */
    bool AllowLegacyBlocks(unsigned nHeight) const
    {
        return nHeight == 0;
    }
};
} // namespace Consensus

#endif // BITCOIN_CONSENSUS_PARAMS_H
