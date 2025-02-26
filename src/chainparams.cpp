// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2014-2018 The Crown developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"

#include "random.h"
#include "util.h"
#include "utilstrencodings.h"

#include <boost/assign/list_of.hpp>

#include <assert.h>
#include <limits>

using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion .SetGenesisVersion(nVersion);
    genesis.vtx.push_back(txNew);
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = genesis.BuildMerkleTree();
    return genesis;
}

static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "The inception of Crowncoin 10/Oct/2014";
    const CScript genesisOutputScript = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

static CBlock CreateDevNetGenesisBlock(const uint256 &prevBlockHash, const std::string& devNetName, uint32_t nTime, uint32_t nNonce, uint32_t nBits, const CAmount& genesisReward)
{
    assert(!devNetName.empty());

    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 1 << std::vector<unsigned char>(devNetName.begin(), devNetName.end());
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = CScript() << OP_RETURN;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion.SetGenesisVersion(4);
    genesis.vtx.push_back(txNew);
    genesis.hashPrevBlock = prevBlockHash;
    genesis.hashMerkleRoot = genesis.BuildMerkleTree();
    return genesis;
}

static CBlock FindDevNetGenesisBlock(const CChainParams& params, const CBlock &prevBlock, const CAmount& reward)
{
    std::string devNetName = GetDevNetName();
    assert(!devNetName.empty());

    CBlock block = CreateDevNetGenesisBlock(prevBlock.GetHash(), devNetName.c_str(), prevBlock.nTime + 1, 0, prevBlock.nBits, reward);

    arith_uint256 bnTarget;
    bnTarget.SetCompact(block.nBits);

    for (uint32_t nNonce = 0; nNonce < std::numeric_limits<uint32_t>::max(); nNonce++) {
        block.nNonce = nNonce;

        uint256 hash = block.GetHash();
        if (UintToArith256(hash) <= bnTarget)
            return block;
    }

    // This is very unlikely to happen as we start the devnet with a very low difficulty. In many cases even the first
    // iteration of the above loop will give a result already
    error("FindDevNetGenesisBlock: could not find devnet genesis block for %s", devNetName);
    assert(false);
}

/**
 * Main network
 */

//! Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress> &vSeedsOut, const SeedSpec6 *data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7*24*60*60;
    for (unsigned int i = 0; i < count; i++)
    {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

static Checkpoints::MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        (  150000, uint256S("0x0000000000012276edd1465babe6d059a3b65b2503b6d9bb4ae5839f5eabac06") )
        (  300000, uint256S("0x00000000000059901c2ea32bc486d91a355a8fe362e34fc3d10c45bb5e5ca79d") )
        (  450000, uint256S("0x0000000000001db791c8581be592b13f756913962b73514a3a636f7a1637a18f") )
        (  600000, uint256S("0x84e2277d1dc957ae41869498311937bdcedce7e48fa33f962b9b9e9c16df5410") )
        (  750000, uint256S("0xac8bf6801d4e51201993d4c623f2e72000781087f508d60d4f307d7928b912d1") )
        (  900000, uint256S("0xbdf0bcfe3ada671b64526854af8cb7f6f52c1489446e26268e70fbee1e72be5f") )
        ( 1050000, uint256S("0x15c897620ab3ee017262add6a49df0510897498f71ec1bc251e3849bb44b633d") )
        ( 1200000, uint256S("0xd6ef0624668a014810547206b6c5191d463fa32d6191dfdb283f14c842fa96c2") )
        ( 1350000, uint256S("0xaf5001f0cbc13e47c7d3c6cea6f0db8dc92a03688031f3ac330c43f5b29f46b3") )
        ( 1500000, uint256S("0x278a333b28cbfa557362ab77e83815ee203cadd79d76a7a92be1782e6e70a356") )
        ( 1650000, uint256S("0x31885f425a43ea15804907001affd696d28d3e7b991dcadc937e71f52c09b6a7") )
        ( 1800000, uint256S("0x9ad4a139ff2e034bafd9647cae7651a34d860afdc483925293c9b7933880a959") )
        ( 1950000, uint256S("0xf1cdba6a857e230ffba2ac7ff6dfab66172255840f792525b758ca7a6bb1e191") )
        ( 2100000, uint256S("0x4657b72edb700d09c6a86429fdfa668a044441b7d68b5090a450363ec6b44395") )
        ( 2250000, uint256S("0x3eb75e85795321bbffcf1781f86fd3135fdc6a14efc71b55e41149ed2048d87d") )
        ( 2400000, uint256S("0x5aa66e017e798454a4b9774bf6c478047e7db0c405b171e6dff2cde9fb8cdd68") )
        ( 2550000, uint256S("0x09ba5b0cfb8aece2c08fc48c4dfc56af7a3d6e811ebf11de85e898894bd319f4") )
        ( 2700000, uint256S("0x81f44f3061a8a61e5729b85522176a931bea90bb27d638bf892c02373c4b554d") )
        ( 2850000, uint256S("0xc54e006c95c5412518dbd62bad1a51b308e70a0637e97bbcc53ee35b77de07a8") )
        ( 3000000, uint256S("0xd27d72dbb83ba7662ad1a022a61ae484e3853a336cd55578dfea3144470344da") )
        ( 3150000, uint256S("0xbe94e6f027f0606739b8b7351989c06030313ae9978d8b4f56488dc75d3ec528") )
        ( 3300000, uint256S("0xe6258fc68a0bbf4d421b347f9aafb17a79ff0f5f83e8ae89d1eb67e1214459df") )
        ( 3450000, uint256S("0xe35a1c54603fa6a0fb3746f61b283eef1b51384c6c3627a2a22dd19d79fdd643") )
        ( 3600000, uint256S("0xad9659bab6547d218ac81f537b9111696be7817efa56b36899a3dfc413ed8b79") )
        ( 3750000, uint256S("0xe9c1c465b132fbafeff92c3039698571a25f4347bc37fdd2bec30619f29e8991") )
        ( 3900000, uint256S("0x9985e0c9260313bacf5a4dc96456501922643e73896975caf39637fe3bfd2f10") )
        ( 4100000, uint256S("0x39cef65984472b0a4e5e62120f227496109b5b34f75db811ed482305821f8543") )
        ;
static const Checkpoints::CCheckpointData data = {
        &mapCheckpoints,
        1408905730, // * UNIX timestamp of last checkpoint block
        1152752,    // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
        1540        // * estimated number of transactions per day after checkpoint
    };

static Checkpoints::MapCheckpoints mapCheckpointsTestnet =
        boost::assign::map_list_of
        ( 0, uint256S("0x0000000085370d5e122f64f4ab19c68614ff3df78c8d13cb814fd7e69a1dc6da"))
        ;
static const Checkpoints::CCheckpointData dataTestnet = {
        &mapCheckpointsTestnet,
        1412760826, // * UNIX timestamp of last checkpoint block
        0,          // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
        0           // * estimated number of transactions per day after checkpoint
    };

static Checkpoints::MapCheckpoints mapCheckpointsDevnet =
        boost::assign::map_list_of
        ( 0, uint256S("0x0080ad356118a9ab8db192db66ef77146cc36d958f959251feace550e4ca3d1446"))
        ;
static const Checkpoints::CCheckpointData dataDevnet = {
        &mapCheckpointsDevnet,
        1412760826, // * UNIX timestamp of last checkpoint block
        0,          // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
        0           // * estimated number of transactions per day after checkpoint
    };

static Checkpoints::MapCheckpoints mapCheckpointsRegtest =
        boost::assign::map_list_of
        ( 0, uint256S("0x231de73ec08234a4adff3c71e57271a13fa73f5ae1ca6b0ded89275e557a6207"))
        ;
static const Checkpoints::CCheckpointData dataRegtest = {
        &mapCheckpointsRegtest,
        1296688602, // * UNIX timestamp of last checkpoint block
        0,          // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
        0           // * estimated number of transactions per day after checkpoint
    };

class CMainParams : public CChainParams {
public:
    CMainParams() {
        networkID = CBaseChainParams::MAIN;
        strNetworkID = "main";
        /** 
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */
        pchMessageStart[0] = 0xb8;
        pchMessageStart[1] = 0xeb;
        pchMessageStart[2] = 0xb3;
        pchMessageStart[3] = 0xdf;
        vAlertPubKey = ParseHex("04977aae0411f4e1757e8682c87ee79180ad577ef0351054e6cda5c9381fcd8c7333e88ac250d3ab3e3aafd5d1c1d946f2ca62372db7f35c84398a878aa145f09a");
        nDefaultPort = 9340;
        bnProofOfWorkLimit = ~arith_uint256(0) >> 32;  // Crown starting difficulty is 1 / 2^12
        nSubsidyHalvingInterval = 2100000;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 0;
        nTargetTimespan = 14 * 24 * 60 * 60; // Crown: 2 weeks
        nTargetSpacing = 1 * 60; // Crown: 1 minutes
        nMaxTipAge = 6 * 60 * 60;

        nBlockPoSStart = 2330000;
        nAuxpowChainId = 20;
        nPoSChainId = 22;
        nStakePointerValidityPeriod = 4320; //Stake pointers are valid to stake with for the next 3 day worth of blocks
        nMaxReorgDepth = 100;
        nKernelModifierOffset = 100; //Number blocks before the stake pointer is the kernel modifier from
        nChainStallDuration = 60*60; //Spacing between blocks that will consider the chain as "stalled"

        /**
         * Build the genesis block. Note that the output of the genesis coinbase cannot
         * be spent as it did not originally exist in the database.
         * 
         * CBlock(hash=00000ffd590b14, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=e0028e, nTime=1390095618, nBits=1e0ffff0, nNonce=28917698, vtx=1)
         *   CTransaction(hash=e0028e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
         *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d01044c5957697265642030392f4a616e2f3230313420546865204772616e64204578706572696d656e7420476f6573204c6976653a204f76657273746f636b2e636f6d204973204e6f7720416363657074696e6720426974636f696e73)
         *     CTxOut(nValue=50.00000000, scriptPubKey=0xA9037BAC7050C479B121CF)
         *   vMerkleTree: e0028e
         */
        const char* pszTimestamp = "The inception of Crowncoin 10/Oct/2014";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 10 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock.SetNull();
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion.SetGenesisVersion(1);
        genesis.nTime    = 1412760826;
        genesis.nBits    = 0x1d00ffff;
        genesis.nNonce   = 1612467894;


        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256S("0x0000000085370d5e122f64f4ab19c68614ff3df78c8d13cb814fd7e69a1dc6da"));
        assert(genesis.hashMerkleRoot == uint256S("0x80ad356118a9ab8db192db66ef77146cc36d958f959251feace550e4ca3d1446"));

/** Main net nameservers defined here, they are geographic strategic and redundant through 2 domains.
* The nameservers are location-XXseedNS.crownplatform. net/org they will point to main net dns seed VPS's
* through location-XX.seeder.crowncoin.net which can be set up by community members, and also wil be set up by the infra team.
* Community members wanting to contribute a VPS will request (outside of public view) for a DNS entry to be made to the main nameservers for their dns seed's IP,
* getting help setting it up from the infra team.
*/


        vSeeds.push_back(CDNSSeedData("eur1-crwdns", "europe-01seedns.crowncoin.org"));
        vSeeds.push_back(CDNSSeedData("eur2-crwdns", "europe-02seedns.crowncoin.net"));
        vSeeds.push_back(CDNSSeedData("nam1-crwdns", "canada-01seedns.crowncoin.org"));
        vSeeds.push_back(CDNSSeedData("ltm1-crwdns", "latam-01seedns.crowncoin.net"));
        vSeeds.push_back(CDNSSeedData("sea1-crwdns", "SEAsia-01seedns.crowncoin.org"));
        vSeeds.push_back(CDNSSeedData("pac1-crwdns", "pacific-01seedns.crowncoin.net"));


        // Crown addresses start with 'CRW'
        base58Prefixes[PUBKEY_ADDRESS] = list_of(0x01)(0x75)(0x07).convert_to_container<std::vector<unsigned char> >();
        base58PrefixesOld[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,0);
        // Crown script addresses start with 'CRM'
        base58Prefixes[SCRIPT_ADDRESS] = list_of(0x01)(0x74)(0xF1).convert_to_container<std::vector<unsigned char> >();
        base58PrefixesOld[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,28);                   // Crown script addresses start with 'C'
        base58Prefixes[SECRET_KEY]     = std::vector<unsigned char>(1,128);                  // Crown private keys start with '5'
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_COIN_TYPE]  = list_of(0x80000048).convert_to_container<std::vector<unsigned char> >();             // Crown BIP44 coin type is '72'

        // Identity addresses start with 'CRP'
        base58Prefixes[IDENTITY_ADDRESS] = list_of(0x01)(0x74)(0xF6).convert_to_container<std::vector<unsigned char> >();
        // App service addresses start with 'CRA'
        base58Prefixes[APP_SERVICE_ADDRESS] = list_of(0x01)(0x74)(0xD5).convert_to_container<std::vector<unsigned char> >();
        // Title addresses start with 'CRT'
        base58Prefixes[TITLE_ADDRESS] = list_of(0x01)(0x75)(0x00).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

        nPoolMaxTransactions = 3;
        strSporkKey = "0393a69774e3327f0a8618b9362f7e413569f687f0f35ddb0444b6ebcb0b116f25";
        strDevfundAddress = "16tg5tuZrPKoBwfbmj2tmiEPhVPzyn3gtP";
        strLegacySignerDummyAddress = "18WTcWvwrNnfqeQAn6th9QQ2EpnXMq5Th8";
        nStartMasternodePayments = 1403728576; //Wed, 25 Jun 2014 20:36:16 GMT
    }

    const Checkpoints::CCheckpointData& Checkpoints() const 
    {
        return data;
    }
    int AuxpowStartHeight() const
    {
        return 453273;
    }

    bool StrictChainId() const
    {
        return true;
    }

    bool AllowLegacyBlocks(unsigned nHeight) const
    {
        return static_cast<int> (nHeight) < AuxpowStartHeight();
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams
{
public:
    CTestNetParams()
    {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";
        pchMessageStart[0] = 0x0f;
        pchMessageStart[1] = 0x18;
        pchMessageStart[2] = 0x0e;
        pchMessageStart[3] = 0x06;
        vAlertPubKey = ParseHex("04977aae0411f4e1757e8682c87ee79180ad577ef0351054e6cda5c9381fcd8c7333e88ac250d3ab3e3aafd5d1c1d946f2ca62372db7f35c84398a878aa145f09a");
        nDefaultPort = 19340;
        nEnforceBlockUpgradeMajority = 51;
        nRejectBlockOutdatedMajority = 75;
        nToCheckBlockUpgradeMajority = 100;
        nSubsidyHalvingInterval = 130000;
        nMinerThreads = 0;
        nTargetTimespan = 2 * 24 * 60 * 60;  // 2 days
        nTargetSpacing = 1.5 * 60;      // 1.5 minutes
        nMaxTipAge = 0x7fffffff;
        nBlockPoSStart = 141000;

        //! Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime    = 1412760826;
        genesis.nNonce   = 1612467894;

//	if (true && genesis.GetHash() != hashGenesisBlock)
//                       {
//                           printf("Searching for genesis block...\n");
//                           arith_uint256 hashTarget = arith_uint256().SetCompact(genesis.nBits);
//                           arith_uint256 thash;
//                           while (true)
//                           {
//                               thash = UintToArith256(genesis.GetHash());
//                               if (thash <= hashTarget)
//                                 break;
//                               if ((genesis.nNonce & 0xFFF) == 0)
//                               {
//                                   printf("nonce %08X: hash = %s (target = %s)\n", genesis.nNonce, thash.ToString().c_str(), hashTarget.ToString().c_str());
//                               }
//                               ++genesis.nNonce;
//                               if (genesis.nNonce == 0)
//                               {
//                                   printf("NONCE WRAPPED, incrementing time\n");
//                                   ++genesis.nTime;
//                               }
//                           }
//                           printf("genesis.nTime = %u \n", genesis.nTime);
//                           printf("genesis.nNonce = %u \n", genesis.nNonce);
//                           printf("genesis.nVersion = %u \n", genesis.nVersion);
//                           printf("genesis.GetHash = %s\n", genesis.GetHash().ToString().c_str()); //first this, then comment this line out and uncomment the one under.
//                           printf("genesis.hashMerkleRoot = %s \n", genesis.hashMerkleRoot.ToString().c_str()); //improvised. worked for me, to find merkle root/
//                       }

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256S("0x0000000085370d5e122f64f4ab19c68614ff3df78c8d13cb814fd7e69a1dc6da"));
        assert(genesis.hashMerkleRoot == uint256S("0x80ad356118a9ab8db192db66ef77146cc36d958f959251feace550e4ca3d1446"));

        vFixedSeeds.clear();
        vSeeds.clear();
// Testnet seeders update is outside of current hotfix scope
        vSeeds.push_back(CDNSSeedData("fra-testnet-crwdns", "fra-testnet-crwdns.crowndns.info"));
        vSeeds.push_back(CDNSSeedData("blr-testnet-crwdns", "blr-testnet-crwdns.crowndns.info"));
        vSeeds.push_back(CDNSSeedData("sgp-testnet-crwdns", "sgp-testnet-crwdns.crowndns.info"));
        vSeeds.push_back(CDNSSeedData("lon-testnet-crwdns", "lon-testnet-crwdns.crowndns.info"));
        vSeeds.push_back(CDNSSeedData("nyc-testnet-crwdns", "nyc-testnet-crwdns.crowndns.info"));
        vSeeds.push_back(CDNSSeedData("tor-testnet-crwdns", "tor-testnet-crwdns.crowndns.info"));
        vSeeds.push_back(CDNSSeedData("sfo-testnet-crwdns", "sfo-testnet-crwdns.crowndns.info"));
        vSeeds.push_back(CDNSSeedData("ams-testnet-crwdns", "ams-testnet-crwdns.crowndns.info"));

        // Testnet crown addresses start with 'tCRW'
        base58Prefixes[PUBKEY_ADDRESS] = list_of(0x01)(0x7A)(0xCD)(0x67).convert_to_container<std::vector<unsigned char> >();
        base58PrefixesOld[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        // Testnet crown script addresses start with 'tCRM'
        base58Prefixes[SCRIPT_ADDRESS] = list_of(0x01)(0x7A)(0xCD)(0x51).convert_to_container<std::vector<unsigned char> >();
        base58PrefixesOld[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);                    // Testnet crown script addresses start with '8' or '9'
        base58Prefixes[SECRET_KEY]     = std::vector<unsigned char>(1,239);                    // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_COIN_TYPE]  = list_of(0x80000001).convert_to_container<std::vector<unsigned char> >();             // Testnet crown BIP44 coin type is '5' (All coin's testnet default)

        // Identity addresses start with 'tCRP'
        base58Prefixes[IDENTITY_ADDRESS] = list_of(0x01)(0x7A)(0xCD)(0x56).convert_to_container<std::vector<unsigned char> >();
        // App service addresses start with 'tCRA'
        base58Prefixes[APP_SERVICE_ADDRESS] = list_of(0x01)(0x7A)(0xCD)(0x35).convert_to_container<std::vector<unsigned char> >();
        // Title addresses start with 'tCRT'
        base58Prefixes[TITLE_ADDRESS] = list_of(0x01)(0x7A)(0xCD)(0x60).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        nPoolMaxTransactions = 2;
        strSporkKey = "025eff2ae991a39bfbcb54b9afceebea0df0fa4ffb6d3708dddba9bc0129c1287b";
        strDevfundAddress = "mr59c3aniaN3qHXej5L8UBsssRZbiUUMnz";
        strLegacySignerDummyAddress = "mr59c3aniaN3qHXej5L8UBsssRZbiUUMnz";
        nStartMasternodePayments = 1420837558; //Fri, 09 Jan 2015 21:05:58 GMT

        nStakePointerValidityPeriod = 3000;
    }
    const Checkpoints::CCheckpointData& Checkpoints() const 
    {
        return dataTestnet;
    }
    int AuxpowStartHeight() const
    {
        return 0;
    }

    bool StrictChainId() const
    {
        return false;
    }

    bool AllowLegacyBlocks(unsigned) const
    {
        return true;
    }
};
static CTestNetParams testNetParams;

/**
 * Devnet
 */
class CDevNetParams : public CChainParams {
public:
    CDevNetParams() {
        networkID = CBaseChainParams::DEVNET;
        strNetworkID = "dev";
        nSubsidyHalvingInterval = 210240;
        bnProofOfWorkLimit = ~arith_uint256(0) >> 8;
        nMaxTipAge = 6 * 60 * 60;

        nTargetTimespan = 2 * 60 * 60;  // 2 hours
        nTargetSpacing = 30;      // 30 seconds

        nEnforceBlockUpgradeMajority = 51;
        nRejectBlockOutdatedMajority = 75;
        nToCheckBlockUpgradeMajority = 100;

        pchMessageStart[0] = 0x0f;
        pchMessageStart[1] = 0x18;
        pchMessageStart[2] = 0x0e;
        pchMessageStart[3] = 0x06;
        vAlertPubKey = ParseHex("04977aae0411f4e1757e8682c87ee79180ad577ef0351054e6cda5c9381fcd8c7333e88ac250d3ab3e3aafd5d1c1d946f2ca62372db7f35c84398a878aa145f09a");
        nDefaultPort = 19342;

        genesis = CreateGenesisBlock(1412760826, 175963287, bnProofOfWorkLimit.GetCompact(), 4, 10 * COIN);
        hashGenesisBlock = genesis.GetHash();
        
        assert(hashGenesisBlock == uint256S("0x0055024546d26a67e2b0a13c86bc841efeeadb7b16155d5ea9a192cf6eeb56e6"));
        assert(genesis.hashMerkleRoot == uint256S("0x80ad356118a9ab8db192db66ef77146cc36d958f959251feace550e4ca3d1446"));

        devnetGenesis = FindDevNetGenesisBlock(*this, genesis, 10 * COIN);
        hashDevnetGenesisBlock = devnetGenesis.GetHash();

        vFixedSeeds.clear();
        vSeeds.clear();

        // Crown addresses start with 'CRW'
        base58Prefixes[PUBKEY_ADDRESS] = list_of(0x01)(0x75)(0x07).convert_to_container<std::vector<unsigned char> >();
        base58PrefixesOld[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,0);
        // Crown script addresses start with 'CRM'
        base58Prefixes[SCRIPT_ADDRESS] = list_of(0x01)(0x74)(0xF1).convert_to_container<std::vector<unsigned char> >();
        base58PrefixesOld[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,28);                   // Crown script addresses start with 'C'
        base58Prefixes[SECRET_KEY]     = std::vector<unsigned char>(1,128);                  // Crown private keys start with '5'
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_COIN_TYPE]  = list_of(0x80000048).convert_to_container<std::vector<unsigned char> >();             // Crown BIP44 coin type is '72'

        // Identity addresses start with 'CRP'
        base58Prefixes[IDENTITY_ADDRESS] = list_of(0x01)(0x74)(0xF6).convert_to_container<std::vector<unsigned char> >();
        // App service addresses start with 'CRA'
        base58Prefixes[APP_SERVICE_ADDRESS] = list_of(0x01)(0x74)(0xD5).convert_to_container<std::vector<unsigned char> >();
        // Title addresses start with 'CRT'
        base58Prefixes[TITLE_ADDRESS] = list_of(0x01)(0x75)(0x00).convert_to_container<std::vector<unsigned char> >();

        fMiningRequiresPeers = false;
        fAllowMinDifficultyBlocks = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;

        nPoolMaxTransactions = 3;

        nBlockPoSStart = 500;
        nAuxpowChainId = 20;
        nPoSChainId = 22;
        nStakePointerValidityPeriod = 10080; // Valid for 7 days, to make sure they are valid after downtime
        nMaxReorgDepth = 100;
        nKernelModifierOffset = 10;
        nChainStallDuration = 60*60;

        nStartMasternodePayments = 1420837558; //Fri, 09 Jan 2015 21:05:58 GMT
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataDevnet;
    }

    int AuxpowStartHeight() const
    {
        return 0;
    }

    bool StrictChainId() const
    {
        return false;
    }

    bool AllowLegacyBlocks(unsigned) const
    {
        return true;
    }
};
static CDevNetParams *devNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams {
public:
    CRegTestParams() {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";
        pchMessageStart[0] = 0xfb;
        pchMessageStart[1] = 0xae;
        pchMessageStart[2] = 0xc6;
        pchMessageStart[3] = 0xdf;
        nSubsidyHalvingInterval = 150;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 1;
        nTargetTimespan = 14 * 24 * 60 * 60; // Crown: 2 weeks
        nTargetSpacing = 1 * 60; // Crown: 1 minutes
        bnProofOfWorkLimit = ~arith_uint256(0) >> 1;
        nMaxTipAge = 6 * 60 * 60;
        genesis.nTime = 1296688602;
        genesis.nBits = 0x207fffff;
        genesis.nNonce = 1;
        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 19445;
        assert(hashGenesisBlock == uint256S("0x231de73ec08234a4adff3c71e57271a13fa73f5ae1ca6b0ded89275e557a6207"));

        vFixedSeeds.clear(); //! Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();  //! Regtest mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;
    }
    const Checkpoints::CCheckpointData& Checkpoints() const 
    {
        return dataRegtest;
    }

    bool StrictChainId() const
    {
        return true;
    }

    bool AllowLegacyBlocks(unsigned) const
    {
        return false;
    }
};
static CRegTestParams regTestParams;

/**
 * Unit test
 */
class CUnitTestParams : public CMainParams, public CModifiableParams {
public:
    CUnitTestParams() {
        networkID = CBaseChainParams::UNITTEST;
        strNetworkID = "unittest";
        nDefaultPort = 18445;
        vFixedSeeds.clear(); //! Unit test mode doesn't have any fixed seeds.
        vSeeds.clear();  //! Unit test mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fAllowMinDifficultyBlocks = false;
        fMineBlocksOnDemand = true;
        nBlockPoSStart = 9999999;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const 
    {
        // UnitTest share the same checkpoints as MAIN
        return data;
    }

    /* StrictChainId() = true inherited from CMainParams.  */

    bool AllowLegacyBlocks(unsigned) const
    {
        /* Allow legacy blocks.  This is to make the unit tests that
           rely on loading predefined block data work.  (In particular,
           miner_tests.cpp.)  */
        return true;
    }

    //! Published setters to allow changing values in unit test cases
    virtual void setSubsidyHalvingInterval(int anSubsidyHalvingInterval)  { nSubsidyHalvingInterval=anSubsidyHalvingInterval; }
    virtual void setEnforceBlockUpgradeMajority(int anEnforceBlockUpgradeMajority)  { nEnforceBlockUpgradeMajority=anEnforceBlockUpgradeMajority; }
    virtual void setRejectBlockOutdatedMajority(int anRejectBlockOutdatedMajority)  { nRejectBlockOutdatedMajority=anRejectBlockOutdatedMajority; }
    virtual void setToCheckBlockUpgradeMajority(int anToCheckBlockUpgradeMajority)  { nToCheckBlockUpgradeMajority=anToCheckBlockUpgradeMajority; }
    virtual void setDefaultConsistencyChecks(bool afDefaultConsistencyChecks)  { fDefaultConsistencyChecks=afDefaultConsistencyChecks; }
    virtual void setAllowMinDifficultyBlocks(bool afAllowMinDifficultyBlocks) {  fAllowMinDifficultyBlocks=afAllowMinDifficultyBlocks; }
    virtual void setProofOfWorkLimit(const arith_uint256& limit) { bnProofOfWorkLimit = limit; }
};
static CUnitTestParams unitTestParams;


static CChainParams *pCurrentParams = 0;

CModifiableParams *ModifiableParams()
{
   assert(pCurrentParams);
   assert(pCurrentParams==&unitTestParams);
   return (CModifiableParams*)&unitTestParams;
}

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams &Params(CBaseChainParams::Network network) {
    switch (network) {
        case CBaseChainParams::MAIN:
            return mainParams;
        case CBaseChainParams::TESTNET:
            return testNetParams;
        case CBaseChainParams::REGTEST:
            return regTestParams;
        case CBaseChainParams::DEVNET:
            assert(devNetParams);
            return *devNetParams;
        case CBaseChainParams::UNITTEST:
            return unitTestParams;
        default:
            assert(false && "Unimplemented network");
            return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network) {
    if (network == CBaseChainParams::DEVNET) {
        devNetParams = new CDevNetParams();
    }

    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}
