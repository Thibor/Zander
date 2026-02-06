#include "intrin.h"
#include "setjmp.h"
#include "stdio.h"
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

using namespace std;

#define W32_BUILD
#undef W32_BUILD

#define GEN_MAGICS
#undef GEN_MAGICS

#define TUNABLE
#undef TUNABLE

#define CPU_TIMING
#undef CPU_TIMING

#define EPD_TESTING
#undef EPD_TESTING

#define ASPIRATION
#ifndef TUNABLE
//#undef ASPIRATION
#endif

#define KNS_TESTING
#undef KNS_TESTING

#define USE_LARGE_PAGES
//#undef USE_LARGE_PAGES

#ifdef KNS_TESTING
#ifdef ASPIRATION
#undef ASPIRATION
#endif
#define KN_FACTOR 8
#endif

#define uint8 unsigned __int8
#define sint8 __int8
#define uint16 unsigned __int16
#define sint16 __int16
#define uint32 unsigned __int32
#define sint32 __int32
#define uint64 unsigned __int64
#define sint64 __int64
#define NAME "Zander"
#define VERSION "2026-02-04"

#define Convert(x,type) ((type)(x))

#define Abs(x) ((x) > 0 ? (x) : (-(x)))
#define Sgn(x) ((x) == 0 ? 0 : ((x) > 0 ? 1 : (-1)))
#define Min(x,y) ((x) < (y) ? (x) : (y))
#define Max(x,y) ((x) > (y) ? (x) : (y))
#define True(x) ((x) != 0)
#define False(x) ((x) == 0)
#define Even(x) False((x) & 1)
#define Odd(x) True((x) & 1)
#define Eq(x,y) ((x) == (y))
#define Combine(x,y) ((x) | ((y) << 16))
#define Compose(x,y) ((x) + ((y) << 16))
#define Opening(x) Convert((x) & 0xFFFF,sint16)
#define Endgame(x) (((x >> 15) & 1) + Convert((x) >> 16,sint16))

#define File(x) ((x) & 7)
#define Rank(x) ((x) >> 3)
#define NDiag(x) (7 - File(x) + Rank(x))
#define SDiag(x) (File(x) + Rank(x))
#define Dist(x,y) Max(Abs(Rank(x)-Rank(y)),Abs(File(x)-File(y)))

#define Bit(x) (Convert(1,uint64) << (x))
#define Cut(x) (x &= (x) - 1)
#define Multiple(x) True((x) & ((x) - 1))
#define Single(x) False((x) & ((x) - 1))
#define Add(x,bit) (x |= Bit(bit))

#define Move(from,to) (((from) << 6) | (to))
#define From(move) (((move) >> 6) & 0x3f)
#define To(move) ((move) & 0x3f)

#define Interior Convert(0x007E7E7E7E7E7E00,uint64)
#define Boundary (~Interior)
#define WhiteArea Convert(0x00000000FFFFFFFF,uint64)
#define BlackArea (~WhiteArea)
#define LightArea Convert(0x55AA55AA55AA55AA,uint64)
#define DarkArea (~LightArea)
#define FileA Convert(0x0101010101010101,uint64)
#define Line0 Convert(0x00000000000000FF,uint64)

#define High32(x) ((x) >> 32)
#define Low32(x) Convert(x,uint32)

#define White 0
#define Black 1
#define WhitePawn 2
#define BlackPawn 3
#define WhiteKnight 4
#define BlackKnight 5
#define WhiteLight 6
#define BlackLight 7
#define WhiteDark 8
#define BlackDark 9
#define WhiteRook 10
#define BlackRook 11
#define WhiteQueen 12
#define BlackQueen 13
#define WhiteKing 14
#define BlackKing 15

#define Slider 0x3FC0
#define IsSlider(x) True(Slider & Bit(x))

#define CanCastle_OO 1
#define CanCastle_oo 2
#define CanCastle_OOO 4
#define CanCastle_ooo 8

#define FlagCastling 0x1000
#define FlagEP 0x2000
#define FlagPKnight 0x4000
#define FlagPLight 0x6000
#define FlagPDark 0x8000
#define FlagPRook 0xA000
#define FlagPQueen 0xC000

#define IsPromotion(move) True((move) & 0xC000)
#define IsCastling(move) True((move) & 0x1000)
#define IsEP(move) Eq((move) & 0xF000, 0x2000)
#define Promotion(move,side) ((side) + (((move) & 0xF000) >> 12)) 

#define OutputDepth(depth) (((depth) - 16)/2)
#define StartPos "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define B1 "r4rk1/ppp2p2/1bnpbq1p/4p1p1/2B1Pn2/2PP2B1/PPNNQPPP/R4RK1 w - - 0 14"
#define B2 "1r6/1p1n4/3p2r1/pP2p2k/P3Pp1p/3N1P1P/R5P1/2R4K w - - 0 36"
#define B3 "7R/3k4/3p4/4p3/Pr2Pp1p/5P1P/6PK/8 w - - 0 56"
#define B4 "2r2rk1/1bq1npp1/p1n1p2p/1pp1P3/4B3/P1N2N2/1PP1QPPP/3RR1K1 b - - 0 15"
#define B5 "7k/2qr1pp1/p3p2p/n3P3/P1p3Q1/1pPnNN1P/1P3PP1/1R4K1 w - - 0 33"
#define B6 "7k/5Qp1/pq2n2p/n2rPN2/2p3N1/1pP4P/1P4PK/1R6 b - - 0 46"
#define B7 "rn1r2k1/1p3p1p/1p1b2p1/2ppN3/3PnP2/P1PR3B/1P4PP/R1B3K1 w - - 0 19"
#define B8 "1r6/5pk1/8/2N4p/3p1Pp1/2p1n1P1/4B2P/2R3K1 w - - 0 41"
#define B9 "rr4k1/2q1bpp1/2ppn2p/p3p3/PPQ1P3/2P1BN1P/5PP1/R2R2K1 w - - 0 21"
#define B10 "3r2k1/4qpp1/PP3n1p/1QN1p3/2P1P3/7P/5PP1/6K1 w - - 0 37"
#define B11 "r1b1k2r/1pqpb1pp/p1n1pn2/5p2/1PP5/P1N1PN2/1B3PPP/R2QKB1R w KQkq - 0 10"
#define B12 "1r4k1/1p5p/4pq1r/1PPp1p1b/R7/1N2P1P1/3Q3P/2R3K1 b - - 0 37"
#define B13 "4b1k1/2q5/4R3/3p1p2/4r3/4PNP1/5K2/Q7 w - - 0 52"
#define B14 "r4rk1/pp1bppbp/1q1p1np1/4n3/2PNP3/2N1BP2/PP1QB1PP/R4RK1 b - - 0 12"
#define B15 "4r3/1pqbrpkp/3p1pp1/4n3/pPPNP3/P4P1P/3Q2P1/2R1RB1K b - - 0 36"
#define B16 "2r5/7p/1p5k/1P3pp1/2P1bP2/P3R1KP/6P1/5B2 b - - 0 61"
#define B17 "8/8/PR6/1P2k3/2P5/7r/2K5/8 b - - 0 80"
#define B18 "3rqbk1/1p4p1/r3ppNp/3n4/1nNB1P2/4P1Q1/1R4PP/1R4K1 b - - 0 28"
#define B19 "r3r1k1/pp2ppbp/2b1n1p1/q1pNn3/2P1P3/1P2B2P/P1Q1BPP1/1R1RN1K1 b - - 0 18"
#define B20 "8/7p/p1pk2p1/4pp2/2P2n1P/1PN2PP1/P4K2/8 b - - 0 44"
#define B21 "8/8/p3pk1p/5pp1/2PK4/1P4P1/P7/8 w - - 0 0"
#define B22 "r5k1/q2b1ppp/2p2n2/Pr1p4/1P1Rp3/1NQ1P1P1/5PBP/R5K1 b - - 0 25"
#define B23 "r1bqk2r/pp2bppp/2n5/3pp2n/1P6/2PQBNP1/P3PPBP/RN3RK1 w kq - 0 12"
#define B24 "5k2/8/5pb1/8/3n1KBR/7P/8/8 b - - 0 73"
#define B25 "r2q1rk1/2p1npp1/1pbppn1p/2P5/P2P4/4PN1P/PB2BPP1/R2Q1RK1 w - - 0 15"
#define B26 "8/r4pk1/4p2p/2Np2p1/3P2P1/1R1BP1KP/nn1r1P2/R7 w - - 0 45"
#define B27 "3q1rk1/p1rnppbp/Bpbp2p1/2p5/3P4/2P1P2P/PP1NQPPB/2R2RK1 w - - 0 16"
#define B28 "3r2k1/p4q2/2R2pp1/3n4/3B4/r1P2Q2/6P1/5RK1 w - - 0 42"
#define B29 "2r2rk1/pp1nbpp1/2p1pn1p/q1Pp1b2/3P4/1P1N2P1/PB1NPPBP/R2Q1RK1 w - - 0 13"
#define B30 "1r1q2k1/pP3p2/B2np3/3p4/3P4/6P1/1Q4KP/2R5 w - - 0 39"

#define MatWQ 1
#define MatBQ 3
#define MatWR 3 * 3
#define MatBR 3 * 3 * 3
#define MatWL 3 * 3 * 3 * 3
#define MatBL 3 * 3 * 3 * 3 * 2
#define MatWD 3 * 3 * 3 * 3 * 2 * 2
#define MatBD 3 * 3 * 3 * 3 * 2 * 2 * 2
#define MatWN 3 * 3 * 3 * 3 * 2 * 2 * 2 * 2
#define MatBN 3 * 3 * 3 * 3 * 2 * 2 * 2 * 2 * 3
#define MatWP 3 * 3 * 3 * 3 * 2 * 2 * 2 * 2 * 3 * 3
#define MatBP 3 * 3 * 3 * 3 * 2 * 2 * 2 * 2 * 3 * 3 * 9
#define TotalMat (MatWQ+MatBQ+MatWL+MatBL+MatWD+MatBD+2*(MatWR+MatBR+MatWN+MatBN)+8*(MatWP+MatBP)) + 1
#define FlagUnusualMaterial (1 << 30)
// stored in the material field of GData structure

#define halt_check if ((Current - Data) >= 126) {evaluate(); return Current->score;} \
    if (Current->ply > 100) return 0; \
	for (i = 4; i <= Current->ply; i+= 2) if (Stack[sp-i] == Current->key) return 0

const int MatCode[16] = {0,0,MatWP,MatBP,MatWN,MatBN,MatWL,MatBL,MatWD,MatBD,MatWR,MatBR,MatWQ,MatBQ,0,0};

const uint64 File[8] = {FileA,FileA<<1,FileA<<2,FileA<<3,FileA<<4,FileA<<5,FileA<<6,FileA<<7};
const uint64 Line[8] = {Line0,(Line0<<8),(Line0<<16),(Line0<<24),(Line0<<32),(Line0<<40),(Line0<<48),(Line0<<56)};

const uint8 UpdateCastling[64] = {
	0xFF^CanCastle_OOO,0xFF,0xFF,0xFF,0xFF^(CanCastle_OO|CanCastle_OOO),0xFF,0xFF,0xFF^CanCastle_OO,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF^CanCastle_ooo,0xFF,0xFF,0xFF,0xFF^(CanCastle_oo|CanCastle_ooo),0xFF,0xFF,0xFF^CanCastle_oo
};

const uint64 BMagic[64] = {
0x0048610528020080, 0x00c4100212410004, 0x0004180181002010, 0x0004040188108502, 
0x0012021008003040, 0x0002900420228000, 0x0080808410c00100, 0x000600410c500622, 
0x00c0056084140184, 0x0080608816830050, 0x00a010050200b0c0, 0x0000510400800181, 
0x0000431040064009, 0x0000008820890a06, 0x0050028488184008, 0x00214a0104068200, 
0x004090100c080081, 0x000a002014012604, 0x0020402409002200, 0x008400c240128100, 
0x0001000820084200, 0x0024c02201101144, 0x002401008088a800, 0x0003001045009000, 
0x0084200040981549, 0x0001188120080100, 0x0048050048044300, 0x0008080000820012, 
0x0001001181004003, 0x0090038000445000, 0x0010820800a21000, 0x0044010108210110, 
0x0090241008204e30, 0x000c04204004c305, 0x0080804303300400, 0x00a0020080080080, 
0x0000408020220200, 0x0000c08200010100, 0x0010008102022104, 0x0008148118008140, 
0x0008080414809028, 0x0005031010004318, 0x0000603048001008, 0x0008012018000100, 
0x0000202028802901, 0x004011004b049180, 0x0022240b42081400, 0x00c4840c00400020, 
0x0084009219204000, 0x000080c802104000, 0x0002602201100282, 0x0002040821880020, 
0x0002014008320080, 0x0002082078208004, 0x0009094800840082, 0x0020080200b1a010, 
0x0003440407051000, 0x000000220e100440, 0x00480220a4041204, 0x00c1800011084800, 
0x000008021020a200, 0x0000414128092100, 0x0000042002024200, 0x0002081204004200
};

const uint64 RMagic[64] = {
0x00800011400080a6, 0x004000100120004e, 0x0080100008600082, 0x0080080016500080, 
0x0080040008000280, 0x0080020005040080, 0x0080108046000100, 0x0080010000204080, 
0x0010800424400082, 0x00004002c8201000, 0x000c802000100080, 0x00810010002100b8, 
0x00ca808014000800, 0x0002002884900200, 0x0042002148041200, 0x00010000c200a100, 
0x00008580004002a0, 0x0020004001403008, 0x0000820020411600, 0x0002120021401a00, 
0x0024808044010800, 0x0022008100040080, 0x00004400094a8810, 0x0000020002814c21, 
0x0011400280082080, 0x004a050e002080c0, 0x00101103002002c0, 0x0025020900201000, 
0x0001001100042800, 0x0002008080022400, 0x000830440021081a, 0x0080004200010084, 
0x00008000c9002104, 0x0090400081002900, 0x0080220082004010, 0x0001100101000820, 
0x0000080011001500, 0x0010020080800400, 0x0034010224009048, 0x0002208412000841, 
0x000040008020800c, 0x001000c460094000, 0x0020006101330040, 0x0000a30010010028, 
0x0004080004008080, 0x0024000201004040, 0x0000300802440041, 0x00120400c08a0011, 
0x0080006085004100, 0x0028600040100040, 0x00a0082110018080, 0x0010184200221200, 
0x0040080005001100, 0x0004200440104801, 0x0080800900220080, 0x000a01140081c200, 
0x0080044180110021, 0x0008804001001225, 0x00a00c4020010011, 0x00001000a0050009, 
0x0011001800021025, 0x00c9000400620811, 0x0032009001080224, 0x001400810044086a
};

const int BShift[64] = {
    58, 59, 59, 59, 59, 59, 59, 58, 
	59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 57, 57, 57, 57, 59, 59, 
	59, 59, 57, 55, 55, 57, 59, 59,
    59, 59, 57, 55, 55, 57, 59, 59, 
	59, 59, 57, 57, 57, 57, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 59, 
	58, 59, 59, 59, 59, 59, 59, 58
};

const int BOffset[64] = {
    0, 64, 96, 128, 160, 192, 224, 256, 
    320, 352, 384, 416, 448, 480, 512, 544, 
    576, 608, 640, 768, 896, 1024, 1152, 1184, 
    1216, 1248, 1280, 1408, 1920, 2432, 2560, 2592, 
    2624, 2656, 2688, 2816, 3328, 3840, 3968, 4000, 
    4032, 4064, 4096, 4224, 4352, 4480, 4608, 4640, 
    4672, 4704, 4736, 4768, 4800, 4832, 4864, 4896, 
    4928, 4992, 5024, 5056, 5088, 5120, 5152, 5184 
};

const int RShift[64] = {
    52, 53, 53, 53, 53, 53, 53, 52, 
	53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53, 
	53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53, 
	53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53, 
	52, 53, 53, 53, 53, 53, 53, 52
};

const int ROffset[64] = {
    5248, 9344, 11392, 13440, 15488, 17536, 19584, 21632, 
    25728, 27776, 28800, 29824, 30848, 31872, 32896, 33920, 
    35968, 38016, 39040, 40064, 41088, 42112, 43136, 44160, 
    46208, 48256, 49280, 50304, 51328, 52352, 53376, 54400, 
    56448, 58496, 59520, 60544, 61568, 62592, 63616, 64640, 
    66688, 68736, 69760, 70784, 71808, 72832, 73856, 74880, 
    76928, 78976, 80000, 81024, 82048, 83072, 84096, 85120, 
    87168, 91264, 93312, 95360, 97408, 99456, 101504, 103552
};

#define BishopAttacks(sq,occ) (MagicAttacks[BOffset[(sq)] + (((BMagicMask[(sq)] & (occ)) * BMagic[(sq)]) >> BShift[(sq)])])
#define RookAttacks(sq,occ) (MagicAttacks[ROffset[(sq)] + (((RMagicMask[(sq)] & (occ)) * RMagic[(sq)]) >> RShift[(sq)])])
#define QueenAttacks(sq,occ) (BishopAttacks(sq,occ) | RookAttacks(sq,occ))

#define FlagHashCheck (1 << 16) // first 16 bits are reserved for the hash killer
#define FlagHaltCheck (1 << 17)
#define FlagCallEvaluation (1 << 18)
#define FlagNullSearch (1 << 19)
#define FlagNeatSearch (FlagHashCheck | FlagHaltCheck | FlagCallEvaluation)
#define FlagDisableDelta (1 << 20)
#define FlagOnlyCaptures (1 << 21)

#define FlagSkipBadChecks (1 << 0)

const int PawnFile[8] = {-3, -1, 0, 1, 1, 0, -1, -3};
const int KnightCenter[8] = {-3, -1, 0, 1, 1, 0, -1, -3};
const int KnightRank[8] = {-2, -1, 0, 1, 2, 3, 2, 1};
#define KnightTrapped 100
const int BishopCenter[8] = {-3, -1, 0, 1, 1, 0, -1, -3};
#define BishopMainDiag 8
const int RookFile[8] = {-2, -1, 0, 1, 1, 0, -1, -2};
const int QueenCenter[8] = {-3, -1, 0, 1, 1, 0, -1, -3};
const int KingFile[8] = {2, 3, 1, 0, 0, 1, 3, 2};
const int KingCenter[8] = {-3, -1, 0, 1, 1, 0, -1, -3};
#define PawnFileOp 5
#define PawnFileEg -2
#define PawnCenterOwn 3
#define PawnCenterOpp 5
#define KnightCenterOp 9
#define KnightCenterEg 2
#define KnightRankOp 9
#define BishopCenterOp 4
#define BishopCenterEg 2
#define RookFileOp 3
#define RookFileEg 0
#define QueenCenterOp 3
#define QueenCenterEg 3
#define KingFileOp 15
#define KingCenterEg 12

#define PawnValueOp 90
#define PawnValueEg 100
#define KnightValue 325
#define BishopValue 325
#define RookValue 515
#define QueenValue 975
#define MateValue 32760
#define EvalValue 30000

#define KnightMobility Compose(5, 9) // tuned
#define BishopMobility Compose(5, 5) // well tuned
#define RookMobility Compose(2, 3) // tuned
#define QueenMobility Compose(2, 2) // tuned
int KnightOutpost[2][64]; // tuned

// tuned
#define KingQAttack Combine(1, 40)
#define KingRAttack Combine(1, 25)
#define KingBAttack Combine(1, 15)
#define KingNAttack Combine(1, 15)
#define KingAttack Combine(1, 0)

#define RookSeventhRank Compose(5, 20) // well tuned
// hardly tuned
#define RookHOF Compose(2, 3) 
#define RookHOFPLook Compose(1, 2)
#define RookHOFUPLook Compose(2, 3)
#define RookOF Compose(8, 5)
#define RookOFFreeLook Compose(4, 3)
#define RookOFMinorPLook Compose(2, 0)
#define RookOFMinorUPLook Compose(10, 15)
#define RookOFKingAtt Compose(15, 0)
#define RookKingLook Compose(10, 0)

#define PieceOutpost Compose(4, 2) // hardly tuned
#define PieceHanging Compose(9, 5) // tuned
#define PieceUPAttack Compose(7, 10) // tuned

const int KingAttackScale[16] = {0, 1, 4, 9, 16, 25, 36, 49, 64, 64, 64, 64, 64, 64, 64, 64}; // tuned
const int ThreatScale[9] = {0, Compose(20,30), Compose(160,240), Compose(480,720), Compose(480,720), // tuned, though the values are ridiculous...
	Compose(480,720), Compose(480,720), Compose(480,720), Compose(480,720)};
const int AdvantageScale[16] = {16, 15, 14, 12, 9, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5}; // tuned
#ifdef TUNABLE
const int SpaceScore[16] = {0, 0, 5, 15, 30, 50, 70, 100, 130, 160, 190, 220, 250, 280, 310, 340};  
#endif
const int SEEValue[16] = {0, 0, 1, 1, 3, 3, 3, 3, 3, 3, 5, 5, 10, 10, 1000, 1000};
const int BishopPair[9] = {70, 65, 60, 55, 50, 45, 40, 35, 30}; // hardly tuned

const int Shelter[3][8] = { // tuned
	0, 24, 17, 5, 3, 2, 1, 0,
	0, 36, 25, 8, 4, 2, 1, 0,
	0, 48, 33, 11, 5, 2, 1, 0
};
const int Storm[8] = { // hardly tuned
	0, 0, 0, 6, 10, 13, 15, 0
};
#define KingBlocked Compose(7, 15) // hardly tuned

//tuned
const int PawnUP[8] = {0, Compose(2, 3), Compose(4, 4), Compose(6, 5), Compose(7, 5), Compose(8, 6), 0};
const int PawnUR[8] = {0, 0, Compose(2, 2), Compose(3, 2), Compose(4, 3), Compose(4, 3), Compose(4, 3), 0}; 
const int PawnWeak[8] = {0, Compose(15, 10), Compose(10, 7), Compose(7, 5), Compose(7, 5), Compose(10, 7), 0, 0};
const int PawnWeakOp[8] = {0, Compose(15, 0), Compose(12, 0), Compose(10, 0), Compose(8, 0), Compose(6, 0), 0, 0};
const int PawnIsolated[8] = {0, Compose(2, 3), Compose(4, 4), Compose(4, 4), Compose(4, 4), Compose(4, 4), Compose(4, 4), 0};
#define PawnDoubled Compose(5, 7)

const int Passer[8] = {0, Compose(1,3), Compose(4,5), Compose(10,10), Compose(25,20), Compose(60,40), Compose(90,60), 0}; // tuned
// hardly tuned
const int NoSelfPasserBlock[8] = {0, 0, 0, 0, Compose(0,1), Compose(1,4), Compose(3,8), 0};
const int NoOppPasserBlock[8] = {0, 0, Compose(0,1), Compose(1,3), Compose(3,10), Compose(8,25), Compose(15,45), 0};
const int RookPasserBlock[8] = {0, 0, 0, Compose(0,2), Compose(0,8), Compose(0,16), Compose(0,20), 0};
const int PasserPush[8] = {0, 0, 0, Compose(1,2), Compose(2, 3), Compose(3, 5), Compose(5, 8), 0};
const int FreePasser[8] = {0, 0, Compose(0,1), Compose(1,3), Compose(5,10), Compose(10,20), Compose(20,40), 0};
const int RagingPasser[8] = {0, 0, 0, Compose(0,1), Compose(3,7), Compose(7,14), Compose(14,28), 0};  
const int PasserDef[8] = {0, 0, Compose(0,1), Compose(0,3), Compose(0,7), Compose(0,12), Compose(0,20), 0};
const int PasserAtt[8] = {0, Compose(0,1), Compose(0,2), Compose(0,6), Compose(0,14), Compose(0,24), Compose(0,40), 0};
const int DistantPasser[8] = {0, Compose(0,1), Compose(0,2), Compose(0,4), Compose(0,8), Compose(0,16), Compose(0,32), 0};
// tuned
const int ProtectedPasser[8] = {0, 0, Compose(0,2), Compose(2,3), Compose(3,5), Compose(6,10), Compose(12,20), 0};

#define ExclusionSingle 15
#define ExclusionDouble 30

typedef struct {
	uint64 bb[16];
	uint8 square[64];
} GBoard;

__declspec(align(64)) GBoard Board[1];
uint64 Stack[2048];
int sp;
uint64 nodes, check_node;

typedef struct {
	uint64 attacks[64];
	uint64 key, pawn_key, eval_key, all_att[2], pawn_att[2], passer, xray;
	uint8 turn, capture, flags, ply, ep_square, piece;
	sint16 score;
	uint16 move;
	uint16 killer[3];
	uint64 mask;
	int margin, gen_flags;
	int material, pst;
	int *start, *current;
	int stage;
	int moves[239];
} GData;

__declspec(align(64)) GData Data[128];
GData *Current = Data;

typedef struct {
	uint32 key;
	uint32 date;
	uint16 move;
	sint16 low;
	sint16 high;
	uint8 low_depth;
	uint8 high_depth;
} GEntry;

#define initial_hash_size (4 * 1024 * 1024)
int hashSize = initial_hash_size;
int hash_mask = (initial_hash_size - 4);
GEntry * Hash;
int init_flag = 0, virtual_mem = 0;
uint32 date;
#ifndef TUNABLE
#define UpdateDate(old_date,new_date) (old_date = new_date)
#else
#define UpdateDate(old_date,new_date) if (old_date < new_date) old_date = new_date
#endif

typedef struct {
	uint32 key;
	uint8 white, black, w_shelter, b_shelter, passer_w, passer_b;
	int score;
} GPawnEntry;

#define pawn_hash_size (512 * 1024)
#define pawn_hash_mask (pawn_hash_size - 1)
GPawnEntry PawnHash[pawn_hash_size];
GEntry * Entry;
GPawnEntry * PawnEntry;

typedef struct {
	uint32 key;
	uint32 date;
	uint16 depth;
	uint16 move;
	int value;
} GPVEntry;

#define pv_hash_size (1024 * 1024)
#define pv_hash_mask (pv_hash_size - 1)
#define pv_cluster_size 4
GPVEntry PVHash[pv_hash_size + pv_cluster_size - 1];
GPVEntry * PVEntry;

uint64 Forward[2][8];
uint64 HLine[64];
uint64 VLine[64];
uint64 NDiag[64];
uint64 SDiag[64];
uint64 RMask[64];
uint64 BMask[64];
uint64 QMask[64];
uint64 BMagicMask[64];
uint64 RMagicMask[64];
uint64 QMagicMask[64];
uint64 NAtt[64];
uint64 SArea[64];
uint64 DArea[64];
uint64 NArea[64];
uint64 PAtt[2][64];
uint64 PMove[2][64];
uint64 PWay[2][64];
uint64 PIsolated[64];
uint64 PSupport[2][64];
uint64 Between[64][64];
uint64 FullLine[64][64];

uint64 MagicAttacks[107648];
uint32 Material[TotalMat];
/* bit description:
0 - 15 (16): signed score
16 - 19 (4): unsigned white multiplier
20 - 23 (4): unsigned black multiplier
24 - 28 (5): unsigned phase
29 - 32 (3): flags */
#define MatLookForDrawW (1 << 29)
#define MatLookForDrawB (1 << 30)
int PST[16][64];

uint64 seed = 1;
uint64 TurnKey;
uint64 PieceKey[16][64];
uint64 CastleKey[16];
uint64 EPKey[8];

uint8 PieceFromChar[256];
uint16 PV[64];
char info_string[1024];
char pv_string[1024];
char score_string[16];
char input_string[65536];
int MultiPV[256];
int pvp;
int pv_length;
int best_move, best_score;
GBoard SaveBoard[1];
GData SaveData[1];

#define MaxHistory 255
uint16 History[16][64];
#define HistoryGain(depth) (1 | (1 << 8))
#define HistoryLoss(depth) (1)
#define History(piece,to) Convert(((History[piece][to] & 0xFF00) / (History[piece][to] & 0x00FF)) << 16,int)
#define HistoryGood(move,depth) \
    if ((History[Board->square[From(move)]][To(move)] & 0x00FF) < MaxHistory) History[Board->square[From(move)]][To(move)] += HistoryGain(depth); \
	else History[Board->square[From(move)]][To(move)] = ((History[Board->square[From(move)]][To(move)] & 0xFEFE) >> 1) + HistoryGain(depth)
#define HistoryBad(move,depth) \
	if ((History[Board->square[From(move)]][To(move)] & 0x00FF) < MaxHistory) History[Board->square[From(move)]][To(move)] += HistoryLoss(depth); \
	else History[Board->square[From(move)]][To(move)] = ((History[Board->square[From(move)]][To(move)] & 0xFEFE) >> 1) + HistoryLoss(depth)
const int HistoryCnt[16] = {3, 4, 5, 6, 7, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
#define HistoryMoveNb(depth) (HistoryCnt[Min(15, Max((depth) - 17, 0))])
sint16 Delta[16][64];
#define DeltaPM Delta[Current->piece][To(Current->move)]
#define DeltaM(move) Delta[Board->square[From(move)]][To(move)]
#define UpdateDelta \
	if (DeltaPM > (-Current->score - (Current - 1)->score)) DeltaPM--; \
	else DeltaPM = (-Current->score - (Current - 1)->score)
#define UpdateDeltaM if (False(Current->capture) && False(Current->move & 0xE000) && False(flags & FlagDisableDelta) && True(Current->move)) UpdateDelta
#define DeltaMargin(piece,to) (Delta[piece][to] > Current->margin)

jmp_buf Jump;
HANDLE StreamHandle; 
#ifdef CPU_TIMING
#include "time.h"
HANDLE CurrentHandle;
#endif
#ifdef EPD_TESTING
#include "math.h"
#endif
int Console, HardwarePopCnt;

int TimeLimit1, TimeLimit2;
int DepthLimit, LastDepth, LastTime;
int PVN, Stop, Print, Infinite, MoveTime, SearchMoves, SMPointer, Ponder, Searching, Bad, Change, Early, Easy;
#ifdef TUNABLE
int Aspiration = 1, LMR = 1, PVHashing = 0, ExtendedInfo = 1, HalfPlySearch = 0, Space = 0, Favor = 0;
#endif
#define EasyMargin 4
#define EarlyMargin 9
#define NormalMargin 12
#define PonderRatio 22
#define TimeValue 38
#define InfoLag 5000
#define InfoDelay 1000
sint64 StartTime, InfoTime, CurrTime;
#ifdef EPD_TESTING
sint64 MaxNodes;
#endif
#ifdef KNS_TESTING
sint64 NodeLimit = 0;
#endif
uint16 SMoves[256];

inline unsigned long lsb(uint64 x);
inline unsigned long msb(uint64 x);
inline int popcnt(uint64 x);
template <bool HPopCnt> int popcount(uint64 x);
uint64 BMagicHash(int i, uint64 mask);
uint64 RMagicHash(int i, uint64 mask);
void init();
uint16 rand16();
uint64 random();
void init_keys();
void init_material();
void init_board();
void init_board(const char fen[]);
void init_hash(int mbSize);
void move_to_string(int move, char string[]);
int move_from_string(char string[]);
void pick_pv();
inline void do_capture(int move, int to, int capture, GData * Next);
void do_move(int move);
void undo_move(int move);
void do_null();
void undo_null();
int * gen_captures(uint64 target, int * list);
int * gen_quiet_moves(int * list);
int * gen_delta_moves(int * list);
int * gen_evasions(int * list);
int * gen_checks(uint64 capture_target, int * list);
int * gen_legal_moves();
int see(int move);
int get_move();
void get_attacks();
void eval_pawns();
template <bool HPopCnt> void evaluation();
inline void evaluate();
int is_legal(int move);
void hash_high(int value, int depth);
void hash_low(int move, int value, int depth);
void hash_high_pv(int value, int depth);
void hash_low_pv(int move, int value, int depth);
#ifdef TUNABLE
void hash_high_forced(int value);
void hash_low_forced(int value);
#endif
void hash_exact(int move, int value, int depth);
inline int extension(int move, int pv);
inline int is_check(int move);
inline int is_defence(int move, uint64 target);
int q_search(int beta, int depth, int flags);
int q_evasion(int beta, int depth, int flags);
int search(int beta, int depth, int flags);
int search_evasion(int beta, int depth, int flags);
int exclusion(int beta, int depth, int killer);
int qpv_search(int alpha, int beta, int depth);
int pv_search(int alpha, int beta, int depth);
void root();
int root_search(int alpha, int beta, int depth);
int multipv(int depth);
void send_pv(int depth, int alpha, int beta, int score);
void send_multipv(int depth, int curr_number);
void send_best_move();
void get_position(char string[]);
void get_time_limit(char string[]);
void check_time(int searching);
void init_search();
int input();
void bench(char string[]);
#ifdef EPD_TESTING
void epd(char string[]);
#endif
void uci();

#ifdef GEN_MAGICS
void gen_magics() {
	int sq, index, i, b, p;
	uint64 u, v, occ[4096], att[4096], set[4096];
	FILE * f = fopen("magics.txt","w");
	fprintf(f,"const uint64 BMagic[64] = {\n");
	for (sq = 0; sq < 64; sq++) {
		for (index = 0; index < Bit(64 - BShift[sq]); index++) {
			u = 0;
			for (v = BMagicMask[sq], i = 0; True(v); Cut(v), i++) {
				b = lsb(v);
				if (index & Bit(i)) Add(u,b);
			}
			occ[index] = u;
			att[index] = BishopAttacks(sq,u);
		}
		for (i = 0; i < 1000000000; i++) {
next_b:
			u = random() & random() & random() & 0x00FFFFFFFFFFFFFF;
			memset(set,0,4096 * sizeof(uint64));
			for (index = 0; index < Bit(64 - BShift[sq]); index++) {
				p = (u * (occ[index])) >> BShift[sq];
				if (False(set[p])) set[p] = att[index];
				else if (set[p] != att[index]) goto next_b;
			}
			fprintf(f,"0x");
			for(b = msb(u)/4; b < 15; b++) fprintf(f,"0"); 
			fprintf(f,"%llx",u);
			if (sq < 63) fprintf(f,", ");
			if (Odd(sq)) fprintf(f,"\n"); 
			break;
		}
	}
	fprintf(f,"};\n");
	fprintf(f,"const uint64 RMagic[64] = {\n");
	for (sq = 0; sq < 64; sq++) {
		for (index = 0; index < Bit(64 - RShift[sq]); index++) {
			u = 0;
			for (v = RMagicMask[sq], i = 0; True(v); Cut(v), i++) {
				b = lsb(v);
				if (index & Bit(i)) Add(u,b);
			}
			occ[index] = u;
			att[index] = RookAttacks(sq,u);
		}
		for (i = 0; i < 1000000000; i++) {
next_r:
			u = random() & random() & random() & 0x00FFFFFFFFFFFFFF;
			memset(set,0,4096 * sizeof(uint64));
			for (index = 0; index < Bit(64 - RShift[sq]); index++) {
				p = (u * (occ[index])) >> RShift[sq];
				if (False(set[p])) set[p] = att[index];
				else if (set[p] != att[index]) goto next_r;
			}
			fprintf(f,"0x");
			for(b = msb(u)/4; b < 15; b++) fprintf(f,"0"); 
			fprintf(f,"%llx",u);
			if (sq < 63) fprintf(f,", ");
			if (Odd(sq)) fprintf(f,"\n"); 
			break;
		}
	}
	fprintf(f,"};\n");
	fclose(f);
}
#endif

#ifndef W32_BUILD
inline unsigned long lsb(uint64 x) {
	register unsigned long y;
	_BitScanForward64(&y,x);
	return y;
}

inline unsigned long msb(uint64 x) {
	register unsigned long y;
	_BitScanReverse64(&y,x);
	return y;
}

inline int popcnt(uint64 x) { 
    x = x - ((x >> 1) & 0x5555555555555555);
    x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
    x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0f;
    return (x * 0x0101010101010101) >> 56;
}

template <bool HPopCnt> int popcount(uint64 x) {
	return HPopCnt ? __popcnt64(x) : popcnt(x);
}
#else
inline unsigned long lsb(uint64 x) {
    _asm {
         mov eax, dword ptr x[0]
        test eax, eax
        jz l_high
        bsf eax, eax
        jmp l_ret
        l_high: bsf eax, dword ptr x[4]
        add eax, 20h
        l_ret:
    }
}

inline unsigned long msb(uint64 x) {
	_asm {
        mov eax, dword ptr x[4]
        test eax, eax
        jz l_low
        bsr eax, eax
        add eax, 20h
        jmp l_ret
        l_low: bsr eax, dword ptr x[0]
        l_ret:
    }
}

inline int popcnt(uint64 x) { 
    unsigned int x1, x2;
    x1 = (unsigned int) (x & 0xFFFFFFFF);
    x1 -= (x1 >> 1) & 0x55555555;
    x1 = (x1 & 0x33333333) + ((x1 >> 2) & 0x33333333);
    x1 = (x1 + (x1 >> 4)) & 0x0F0F0F0F;
    x2 = (unsigned int) (x >> 32);
    x2 -= (x2 >> 1) & 0x55555555;
    x2 = (x2 & 0x33333333) + ((x2 >> 2) & 0x33333333);
    x2 = (x2 + (x2 >> 4)) & 0x0F0F0F0F;
    return ((x1 * 0x01010101) >> 24) + ((x2 * 0x01010101) >> 24);
}

template <bool HPopCnt> int popcount(uint64 x) {
	return HPopCnt ? (__popcnt((int)x) + __popcnt(x >> 32)) : popcnt(x);
}
#endif

uint64 BMagicHash(int i, uint64 occ) {
    uint64 free = 0;
    int j;
	if (File(i) < 7 && Rank(i) < 7) {
		occ |= File[7] | Line[7];
        for (j = i + 9; ((j & 7) < 7) && ((occ & ((uint64)1 << j)) == 0); j += 9) free |= (uint64)1 << j;
	    free |= (uint64)1 << j;
	}
	if (File(i) > 0 && Rank(i) < 7) {
		occ |= File[0] | Line[7];
	    for (j = i + 7; ((j & 7) > 0) && ((occ & ((uint64)1 << j)) == 0); j += 7) free |= (uint64)1 << j;
	    free |= (uint64)1 << j;
	}
	if (File(i) > 0 && Rank(i) > 0) {
		occ |= File[0] | Line[0];
	    for (j = i - 9; ((j & 7) > 0) && ((occ & ((uint64)1 << j)) == 0); j -= 9) free |= (uint64)1 << j;
	    free |= (uint64)1 << j;
	}
	if (File(i) < 7 && Rank(i) > 0) {
		occ |= File[7] | Line[0];
	    for (j = i - 7; ((j & 7) < 7) && ((occ & ((uint64)1 << j)) == 0); j -= 7) free |= (uint64)1 << j;
	    free |= (uint64)1 << j;
	}
	return free;
}

uint64 RMagicHash(int i, uint64 occ) {
    uint64 free = 0;
    int j;
	if (File(i) < 7) {
		occ |= File[7];
        for (j = i + 1; ((j & 7) < 7) && ((occ & ((uint64)1 << j)) == 0); j++) free |= (uint64)1 << j;
	    free |= (uint64)1 << j;
	}
	if (File(i) > 0) {
		occ |= File[0];
        for (j = i - 1; ((j & 7) > 0) && ((occ & ((uint64)1 << j)) == 0); j--) free |= (uint64)1 << j;
	    free |= (uint64)1 << j;
	}
	if (Rank(i) < 7) {
		occ |= Line[7];
	    for (j = i + 8; (j < 56) && ((occ & ((uint64)1 << j)) == 0); j += 8) free |= (uint64)1 << j;
	    free |= (uint64)1 << j;
	}
	if (Rank(i) > 0) {
		occ |= Line[0];
        for (j = i - 8; (j > 7) && ((occ & ((uint64)1 << j)) == 0); j -= 8) free |= (uint64)1 << j;
	    free |= (uint64)1 << j;
	}
	return free;
}

void init() {
	int i, j, k, l, n, index, bits, file, rank, op, eg;
	int bit_list[16];
	uint64 u;

	for (i = 0; i < 64; i++) {
		HLine[i] = VLine[i] = NDiag[i] = SDiag[i] = RMask[i] = BMask[i] = QMask[i] = 0;
		BMagicMask[i] = RMagicMask[i] = QMagicMask[i] = NAtt[i] = SArea[i] = DArea[i] = NArea[i] = PIsolated[i] = 0;
		PAtt[0][i] = PAtt[1][i] = PMove[0][i] = PMove[1][i] = PWay[0][i] = PWay[1][i] = PSupport[0][i] = PSupport[1][i] = 0;
		for (j = 0; j < 64; j++) {
			Between[i][j] = 0;
			FullLine[i][j] = 0;
		}
	}
	for (i = 0; i < 64; i++) {
		for (j = 0; j < 64; j++) {
			if (i != j) {
				u = Bit(j);
				if (File(i) == File(j)) VLine[i] |= u;
				if (Rank(i) == Rank(j)) HLine[i] |= u;
				if (NDiag(i) == NDiag(j)) NDiag[i] |= u;
				if (SDiag(i) == SDiag(j)) SDiag[i] |= u;
				if (Dist(i,j) <= 2) {
					DArea[i] |= u;
					if (Dist(i,j) <= 1) SArea[i] |= u;
					if (Abs(Rank(i)-Rank(j)) + Abs(File(i)-File(j)) == 3) NAtt[i] |= u;
				}
				if (j == i + 8) PMove[0][i] |= u;
				if (j == i - 8) PMove[1][i] |= u;
				if (Abs(File(i) - File(j)) == 1) {
					PIsolated[i] |= u;
					if (Rank(j) > Rank(i)) {
						PSupport[1][i] |= u;
				        if (Rank(j) - Rank(i) == 1) PAtt[0][i] |= u;
					} else if (Rank(j) < Rank(i)) {
						PSupport[0][i] |= u;
						if (Rank(i) - Rank(j) == 1) PAtt[1][i] |= u;
					}
				} else if (File(i) == File(j)) {
					if (Rank(j) > Rank(i)) PWay[0][i] |= u;
					else PWay[1][i] |= u;
				}
			}
		}
	}
	for (i = 0; i < 64; i++) {
		RMask[i] = HLine[i] | VLine[i];
		BMask[i] = NDiag[i] | SDiag[i];
		QMask[i] = RMask[i] | BMask[i];
		BMagicMask[i] = BMask[i] & Interior;
		RMagicMask[i] = RMask[i];
		if (File(i) > 0) RMagicMask[i] &= ~File[0];
		if (Rank(i) > 0) RMagicMask[i] &= ~Line[0];
		if (File(i) < 7) RMagicMask[i] &= ~File[7];
		if (Rank(i) < 7) RMagicMask[i] &= ~Line[7];
		QMagicMask[i] = BMagicMask[i] | RMagicMask[i];
		for (j = 0; j < 64; j++) {
			if (NAtt[i] & NAtt[j]) Add(NArea[i],j);
		}
	}
	for (i = 0; i < 64; i++) {
		for (u = QMask[i]; True(u); Cut(u)) {
			j = lsb(u);
			k = Sgn(Rank(j)-Rank(i));
			l = Sgn(File(j)-File(i));
			for (n = i + 8 * k + l; n != j; n += (8 * k + l)) Add(Between[i][j],n);
		}
		for (u = BMask[i]; True(u); Cut(u)) {
			j = lsb(u);
			FullLine[i][j] = BMask[i] & BMask[j];
		}
		for (u = RMask[i]; True(u); Cut(u)) {
			j = lsb(u);
			FullLine[i][j] = RMask[i] & RMask[j];
		}
	}
	for (i = 0; i < 64; i++) {
		bits = 64 - BShift[i];
		for (u = BMagicMask[i], j = 0; True(u); Cut(u), j++) bit_list[j] = lsb(u);
		for (j = 0; j < Bit(bits); j++) {
			u = 0;
			for (k = 0; k < bits; k++)
				if (Odd(j >> k)) Add(u,bit_list[k]);
			index = Convert(BOffset[i] + ((BMagic[i] * u) >> BShift[i]),int);
            MagicAttacks[index] = BMagicHash(i,u);
		}
		bits = 64 - RShift[i];
		for (u = RMagicMask[i], j = 0; True(u); Cut(u), j++) bit_list[j] = lsb(u);
		for (j = 0; j < Bit(bits); j++) {
			u = 0;
			for (k = 0; k < bits; k++)
				if (Odd(j >> k)) Add(u,bit_list[k]);
			index = Convert(ROffset[i] + ((RMagic[i] * u) >> RShift[i]),int);
            MagicAttacks[index] = RMagicHash(i,u);
		}
	}
	for (i = 0; i < 64; i++) {
		file = File(i);
		rank = Rank(i);
		PST[WhitePawn][i] = Compose(PawnFile[file]*PawnFileOp,PawnFile[file]*PawnFileEg);
		PST[WhiteKnight][i] = Compose((KnightCenter[file]+KnightCenter[rank])*KnightCenterOp+
			KnightRank[rank]*KnightRankOp,(KnightCenter[file]+KnightCenter[rank])*KnightCenterEg);
		PST[WhiteLight][i] = Compose((BishopCenter[file]+BishopCenter[rank])*BishopCenterOp,
			(BishopCenter[file]+BishopCenter[rank])*BishopCenterEg);
		PST[WhiteDark][i] = PST[WhiteLight][i];
		PST[WhiteRook][i] = Compose(RookFile[file]*RookFileOp,RookFile[file]*RookFileEg);
		PST[WhiteQueen][i] = Compose((QueenCenter[file]+QueenCenter[rank])*QueenCenterOp,
			(QueenCenter[file]+QueenCenter[rank])*QueenCenterEg);
		PST[WhiteKing][i] = Compose(KingFile[file]*KingFileOp,(KingCenter[file]+KingCenter[rank])*KingCenterEg);
	}
	PST[WhiteKnight][56] -= Compose(KnightTrapped,0);
	PST[WhiteKnight][63] -= Compose(KnightTrapped,0);
	PST[WhitePawn][27] += Compose(PawnCenterOwn,0);
	PST[WhitePawn][28] += Compose(PawnCenterOwn,0);
	PST[WhitePawn][35] += Compose(PawnCenterOpp,0);
	PST[WhitePawn][36] += Compose(PawnCenterOpp,0);
	for (i = 0; i < 8; i++) {
		PST[WhiteLight][i * 9] += Compose(BishopMainDiag, BishopMainDiag);
		PST[WhiteLight][(8 - i) * 7] += Compose(BishopMainDiag, BishopMainDiag);
		PST[WhiteDark][i * 9] += Compose(BishopMainDiag, BishopMainDiag);
		PST[WhiteDark][(8 - i) * 7] += Compose(BishopMainDiag, BishopMainDiag);
	}
	for (i = 0; i < 64; i++) {
		for (j = 3; j < 16; j+=2) {
			op = Opening(PST[j-1][63-i]);
			eg = Endgame(PST[j-1][63-i]);
			PST[j][i] = Compose(-op,-eg);
		}
	}
	for (i = 0; i < 8; i++) {
		Forward[0][i] = Forward[1][i] = 0;
		for (j = 0; j < 8; j++) {
			if (i < j) Forward[0][i] |= Line[j];
			else if (i > j) Forward[1][i] |= Line[j];
		}
	}
	for (i = 0; i < 64; i++) {
		KnightOutpost[0][i] = Compose(Max(20, Opening(PST[WhiteKnight][i])) >> 2, Max(4, Endgame(PST[WhiteKnight][i])) >> 2);
		KnightOutpost[1][i] = Compose(Max(20, -Opening(PST[BlackKnight][i])) >> 2, Max(4, -Endgame(PST[BlackKnight][i])) >> 2);
	}
	for (i = 0; i < 256; i++) PieceFromChar[i] = 0;
    PieceFromChar[66] = 6;
    PieceFromChar[75] = 14;
    PieceFromChar[78] = 4;
    PieceFromChar[80] = 2;
    PieceFromChar[81] = 12;
    PieceFromChar[82] = 10;
    PieceFromChar[98] = 7;
    PieceFromChar[107] = 15;
    PieceFromChar[110] = 5;
    PieceFromChar[112] = 3;
    PieceFromChar[113] = 13;
    PieceFromChar[114] = 11;
}

uint16 rand16() {
	seed = (seed * Convert(6364136223846793005,uint64)) + Convert(1442695040888963407,uint64);
	return Convert((seed >> 32) & 0xFFFF,uint16);
}

uint64 random() {
	uint64 key;
#ifdef GEN_MAGIC
	key = Convert(rand() & 0xFFFF,uint64); key <<= 16;
	key |= Convert(rand() & 0xFFFF,uint64); key <<= 16;
	key |= Convert(rand() & 0xFFFF,uint64); key <<= 16;
	key |= Convert(rand() & 0xFFFF,uint64);
#else
	key = Convert(rand16(),uint64); key <<= 16;
	key |= Convert(rand16(),uint64); key <<= 16;
	key |= Convert(rand16(),uint64); key <<= 16;
	key |= Convert(rand16(),uint64);
#endif
	return key;
}

void init_keys() {
	int i, j;
	TurnKey = random();
	for (i = 0; i < 8; i++) EPKey[i] = random();
	for (i = 0; i < 16; i++) CastleKey[i] = random();
	for (i = 0; i < 16; i++) {
		for (j = 0; j < 64; j++) PieceKey[i][j] = random();
	}
}

void init_material() {
	int wp, bp, wn, bn, wl, bl, wd, bd, wr, br, wq, bq, index, score, phase, wb, bb;
    int w_maj, b_maj, w_maj_score, b_maj_score, w_min, b_min, w_tot_score, b_tot_score, wmul, bmul;
	for (index = 0; index < TotalMat; index++) Material[index] = 0xFFFFFFFF;
	
	for (wq = 0; wq < 3; wq++)
	for (bq = 0; bq < 3; bq++)
	for (wr = 0; wr < 3; wr++)
	for (br = 0; br < 3; br++)
	for (wl = 0; wl < 2; wl++)
	for (bl = 0; bl < 2; bl++)
	for (wd = 0; wd < 2; wd++)
	for (bd = 0; bd < 2; bd++)
	for (wn = 0; wn < 3; wn++)
	for (bn = 0; bn < 3; bn++)
	for (wp = 0; wp < 9; wp++)
	for (bp = 0; bp < 9; bp++) {
		index = wp*MatWP+bp*MatBP+wn*MatWN+bn*MatBN+wl*MatWL+bl*MatBL+wd*MatWD+bd*MatBD+wr*MatWR+br*MatBR+wq*MatWQ+bq*MatBQ;
		if (Material[index] == 0xFFFFFFFF) {
			phase = Min(24, (wn + bn + wl + bl + wd + bd) + 2 * (wr + br) + 4 * (wq + bq));
			wb = wl + wd;
			bb = bl + bd;
			w_min = wb + wn;
	        b_min = bb + bn;
		    w_maj = wq + wr;
	        b_maj = bq + br;
			w_maj_score = 5 * wr + 10 * wq;
            b_maj_score = 5 * br + 10 * bq;
		    w_tot_score = w_maj_score + 3 * w_min;
		    b_tot_score = b_maj_score + 3 * b_min;
			wmul = 15;
			bmul = 15;
			score = ((((PawnValueOp*phase)+(PawnValueEg*(24-phase)))*(wp-bp))/24)+KnightValue*(wn-bn)+BishopValue*(wb-bb)+RookValue*(wr-br)+QueenValue*(wq-bq);
			if (wp <= 1) {
				if (wmul > 9) wmul = 9;
				if (wp == 0) {
					if (wmul > 7) wmul = 7;
				}
			}
			if (bp <= 1) {
				if (bmul > 9) bmul = 9;
				if (bp == 0) {
					if (bmul > 7) bmul = 7;
				}
			}
			if (wp == 0 && w_tot_score - b_tot_score <= 3) {
			    if (wb < 2 || b_min != bn || bn > 1 || b_maj != 0) {
				    if (wmul > 1) wmul = 2;
			    }
		    }
		    if (bp == 0 && b_tot_score - w_tot_score <= 3) {
			    if (bb < 2 || w_min != wn || wn > 1 || w_maj != 0) {
				    if (bmul > 1) bmul = 2;
			    }
		    }
			if (w_maj == 0) {
			    if (w_min == 0) {
                    if (bp > wp) score -= (bp - wp) * PawnValueEg;
			    } else if (w_min == 1 && wb == 1) {
					if (b_min == 1 && bb == 1 && wl != bl) if (wmul > 7) wmul = 7;
				}
			    if (wp == 0) {
				    if (w_min <= 1) wmul = 0;
				    else if (w_min == 2) {
					    if (wn == 2) {
						    if (b_min + b_maj == 0 && bp != 0) {
							    if (wmul > 4) wmul = 4;
						    } else wmul = 0;
					    }
				    }
			    }
		    }
		    if (b_maj == 0) {
			    if (b_min == 0) {
                    if (wp > bp) score += (wp - bp) * PawnValueEg;
			    } else if (b_min == 1 && bb == 1) {
					if (w_min == 1 && wb == 1 && wl != bl) if (bmul > 7) bmul = 7;
				}
			    if (bp == 0) {
				    if (b_min <= 1) bmul = 0;
				    else if (b_min == 2) {
					    if (bn == 2) {
						    if (w_min + w_maj == 0 && wp != 0) {
                                if (bmul > 4) bmul = 4;
						    } else bmul = 0;
					    }
				    }
			    }
		    }
			if (b_maj == 0 && b_min == 0 && bp == 0) {
				if (w_maj >= 1) wmul = 15;
				else if (wn >= 1 && wb >= 1) wmul = 15;
				else if (wl >= 1 && wd >= 1) wmul = 15;
			}
			if (w_maj == 0 && w_min == 0 && wp == 0) {
				if (b_maj >= 1) bmul = 15;
				else if (bn >= 1 && bb >= 1) bmul = 15;
				else if (bl >= 1 && bd >= 1) bmul = 15;
			}
			if (b_tot_score <= 5 && bp == 0 && wq == 1) wmul = 15;
			if (w_tot_score <= 5 && wp == 0 && bq == 1) bmul = 15;
			if (True(wl) && True(wd)) score += BishopPair[wp];
			if (True(bl) && True(bd)) score -= BishopPair[bp];
		    Material[index] = Convert(score,uint16) | (wmul << 16) | (bmul << 20) | (phase << 24);
			if (w_maj == 0 && w_min == wb && wb <= 1) Material[index] |= MatLookForDrawW;
			if (b_maj == 0 && b_min == bb && bb <= 1) Material[index] |= MatLookForDrawB;
		}
	}
}

void init_board() {
	int i;
	uint64 occ;

	occ = 0;
	sp = 0;
	Current = Data;
	Current->material = 0;
	Current->pst = 0;
	Current->key = PieceKey[0][0];
	if (Current->turn) Current->key ^= TurnKey;
	Current->key ^= CastleKey[Current->flags];
	if (Current->ep_square) Current->key ^= EPKey[File(Current->ep_square)];
	Current->pawn_key = 0;
	for (i = 0; i < 16; i++) Board->bb[i] = 0;
	for (i = 0; i < 64; i++) {
		if (Board->square[i]) {
		    Add(Board->bb[Board->square[i]],i);
		    Add(Board->bb[Board->square[i] & 1],i);
		    Add(occ,i);
		    Current->key ^= PieceKey[Board->square[i]][i];
		    if (Board->square[i] < WhiteKnight) Current->pawn_key ^= PieceKey[Board->square[i]][i];
			if (Board->square[i] < WhiteKing) Current->material += MatCode[Board->square[i]];
			else Current->pawn_key ^= PieceKey[Board->square[i]][i];
			Current->pst += PST[Board->square[i]][i];
		}
	}
	for (i = 0; i < 64; i++) {
		if (IsSlider(Board->square[i])) {
			if (Board->square[i] < WhiteRook) Current->attacks[i] = BishopAttacks(i,occ);
			else if (Board->square[i] < WhiteQueen) Current->attacks[i] = RookAttacks(i,occ);
			else Current->attacks[i] = QueenAttacks(i,occ);
		}
	}
	if (popcnt(Board->bb[WhiteKnight]) > 2 || popcnt(Board->bb[WhiteLight]) > 1 || popcnt(Board->bb[WhiteDark]) > 1 
		|| popcnt(Board->bb[WhiteRook]) > 2 || popcnt(Board->bb[WhiteQueen]) > 2) Current->material |= FlagUnusualMaterial; 
	if (popcnt(Board->bb[BlackKnight]) > 2 || popcnt(Board->bb[BlackLight]) > 1 || popcnt(Board->bb[BlackDark]) > 1 
		|| popcnt(Board->bb[BlackRook]) > 2 || popcnt(Board->bb[BlackQueen]) > 2) Current->material |= FlagUnusualMaterial; 
	Current->capture = 0;
	Current->killer[0] = Current->killer[1] = Current->killer[2] = 0;
	Current->ply = 0;
	Stack[sp] = Current->key;
	Searching = 0;
}

void init_board(const char fen[]) {
	int pos, i, j;
	unsigned char c;

	Current = Data;
	memset(Board,0,sizeof(GBoard));
	memset(Current,0,sizeof(GData));
	pos = 0;
	c = fen[pos];
	for (i = 56; i >= 0; i -= 8) {
		for (j = 0; j <= 7; ) {
            if (c <= '8') j += c - '0';
			else {
				Board->square[i+j] = PieceFromChar[c];
				if (Even(SDiag(i+j)) && Eq(Board->square[i+j]/2,3)) Board->square[i+j] += 2;
				j++;
			}
			c = fen[++pos];
		}
		c = fen[++pos];
	}
	if (c == 'b') Current->turn = 1;
	c = fen[++pos]; c = fen[++pos];
    if (c == '-') c = fen[++pos];
	if (c == 'K') { Current->flags |= CanCastle_OO; c = fen[++pos]; }
	if (c == 'Q') { Current->flags |= CanCastle_OOO; c = fen[++pos]; }
	if (c == 'k') { Current->flags |= CanCastle_oo; c = fen[++pos]; }
	if (c == 'q') { Current->flags |= CanCastle_ooo; c = fen[++pos]; }
	c = fen[++pos];
	if (c != '-') {
        i = c + fen[++pos] * 8 - 489;
		j = i ^ 8;
		if (Board->square[i] != 0) i = 0;
		else if (Board->square[j] != (3 - Current->turn)) i = 0;
		else if (Board->square[j-1] != (Current->turn+2) && Board->square[j+1] != (Current->turn+2)) i = 0;
		Current->ep_square = i;
	}
	init_board();
}

void init_hash(int mbSize) {
	if (mbSize < 1)
		mbSize = 1;
	if (mbSize > 1024)
		mbSize = 1024;
	int recSize = sizeof(GEntry);
	hashSize = 1;
	while (hashSize * recSize <= mbSize * 1000000)
		hashSize <<= 1;
	hash_mask = (hashSize - 1)&~3;
	free(Hash);
	Hash = (GEntry*)calloc(hashSize, recSize);
	memset(Hash, 0, hashSize * sizeof(GEntry));
}

void move_to_string(int move, char string[]) { 
	int pos = 0;
    string[pos++] = ((move >> 6) & 7) + 'a';
    string[pos++] = ((move >> 9) & 7) + '1';
    string[pos++] = (move & 7) + 'a';
    string[pos++] = ((move >> 3) & 7) + '1';
    if (IsPromotion(move)) {
        if ((move & 0xF000) == FlagPQueen)  string[pos++] = 'q';
        else if ((move & 0xF000) == FlagPRook)   string[pos++] = 'r';
        else if ((move & 0xF000) == FlagPLight || (move & 0xF000) == FlagPDark) string[pos++] = 'b';
        else if ((move & 0xF000) == FlagPKnight) string[pos++] = 'n';
    }
    string[pos] = 0;
}

int move_from_string(char string[]) { 
	int from, to, move;
    from = ((string[1] - '1') * 8) + (string[0] - 'a');
    to  = ((string[3] - '1') * 8) + (string[2] - 'a');
    move = (from << 6) | to;
    if (Board->square[from] >= WhiteKing && Abs(to - from) == 2) move |= FlagCastling;
    if (True(Current->ep_square) && to == Current->ep_square) move |= FlagEP;
    if (string[4] != 0) {
        if (string[4] == 'q') move |= FlagPQueen;
        else if (string[4] == 'r') move |= FlagPRook;
        else if (string[4] == 'b') {
			if (Odd(to ^ Rank(to))) move |= FlagPLight;
			else move |= FlagPDark;
		} else if (string[4] == 'n') move |= FlagPKnight;
    }
    return move;
}

void pick_pv() {
	GEntry * Entry;
	int i, depth, move;
	if (pvp >= Min(pv_length,64)) {
		PV[pvp] = 0;
		return;
	}
	move = 0;
	depth = -256;
	for (i = 0, Entry = Hash + (High32(Current->key) & hash_mask); i < 4; i++, Entry++) {
		if (Entry->key == Low32(Current->key)) {
			UpdateDate(Entry->date, date);
			if (True(Entry->move) && Entry->low_depth > depth) {
				depth = Entry->low_depth;
				move = Entry->move;
			}
		}
	}
	for (i = 0, PVEntry = PVHash + (High32(Current->key) & pv_hash_mask); i < pv_cluster_size; i++, PVEntry++) {
		if (PVEntry->key == Low32(Current->key)) {
			PVEntry->date = date;
			hash_high_pv(PVEntry->value, PVEntry->depth);
			hash_low_pv(PVEntry->move, PVEntry->value, PVEntry->depth);
			if (PVEntry->depth >= depth && True(PVEntry->move)) {
				depth = PVEntry->depth;
				move = PVEntry->move;
			}
			break;
		}
	}
	evaluate();
	if (move && is_legal(move)) {
		PV[pvp] = move;
		pvp++;
	    do_move(move);
		for (i = 4; i < Current->ply; i+= 2) if (Stack[sp-i] == Current->key) {
			PV[pvp] = 0;
			goto finish;
		}
		pick_pv();
finish:
		undo_move(move);
	} else PV[pvp] = 0;
}

inline void do_capture(int move, int to, int capture, GData * Next) {
	int piece, me, from;
	uint64 u, mask_from, mask_to, occ;
	from = From(move);
	piece = Board->square[from];
	Next->turn = Current->turn ^ 1;
	me = piece & 1;
	Next->capture = capture;
	Board->square[from] = 0;
	Board->square[to] = piece;
	Next->piece = piece;
	mask_from = Bit(from);
	mask_to = Bit(to);
	Board->bb[piece] ^= mask_from;
	Board->bb[me] ^= mask_from;
	Board->bb[capture] ^= mask_to;
	Board->bb[me ^ 1] ^= mask_to;
	Board->bb[piece] |= mask_to;
	Board->bb[me] |= mask_to;
	Next->flags = Current->flags & UpdateCastling[to] & UpdateCastling[from];
	Next->pst = Current->pst + PST[piece][to] - PST[piece][from] - PST[capture][to];
	Next->key = Current->key ^ PieceKey[piece][from] ^ PieceKey[piece][to] ^ PieceKey[capture][to] ^ 
		CastleKey[Current->flags] ^ CastleKey[Next->flags];
	if (capture >= WhiteKnight) Next->pawn_key = Current->pawn_key;
	else Next->pawn_key = Current->pawn_key ^ PieceKey[capture][to];
	Next->material = Current->material - MatCode[capture];
	if (Current->material & FlagUnusualMaterial && capture >= WhiteKnight) {
		if (popcnt(Board->bb[WhiteQueen]) <= 2 && popcnt(Board->bb[BlackQueen]) <= 2) {
			if (popcnt(Board->bb[WhiteLight]) <= 1 && popcnt(Board->bb[BlackLight]) <= 1 && popcnt(Board->bb[WhiteKnight]) <= 2
				&& popcnt(Board->bb[BlackKnight]) <= 2 && popcnt(Board->bb[WhiteRook]) <= 2 && popcnt(Board->bb[BlackRook]) <= 2)
				Next->material ^= FlagUnusualMaterial;
		}
	}
	if (piece < WhiteKnight) {
		Next->pawn_key ^= PieceKey[piece][from] ^ PieceKey[piece][to];
		if (IsPromotion(move)) {
			piece = Promotion(move,me);
			Board->square[to] = piece;
		    Next->material += MatCode[piece] - MatCode[WhitePawn | me];
			if (piece < WhiteRook) {
				if (piece >= WhiteLight && True(Board->bb[piece])) Next->material |= FlagUnusualMaterial;
				if (Multiple(Board->bb[piece])) Next->material |= FlagUnusualMaterial;
			} else if (Multiple(Board->bb[piece])) Next->material |= FlagUnusualMaterial;
			Board->bb[WhitePawn | me] ^= mask_to;
			Board->bb[piece] |= mask_to;
			Next->pst += PST[piece][to] - PST[WhitePawn | me][to];
			Next->key ^= PieceKey[piece][to] ^ PieceKey[WhitePawn | me][to];
			Next->pawn_key ^= PieceKey[WhitePawn | me][to];
		}
		PawnEntry = PawnHash + (High32(Next->pawn_key) & pawn_hash_mask);
	    _mm_prefetch((char *)PawnEntry,_MM_HINT_NTA);
	} else if (piece >= WhiteKing) {
		Next->pawn_key ^= PieceKey[piece][from] ^ PieceKey[piece][to];
		PawnEntry = PawnHash + (High32(Next->pawn_key) & pawn_hash_mask);
	    _mm_prefetch((char *)PawnEntry,_MM_HINT_NTA);
	} else if (capture < WhiteKnight) {
		PawnEntry = PawnHash + (High32(Next->pawn_key) & pawn_hash_mask);
	    _mm_prefetch((char *)PawnEntry,_MM_HINT_NTA);
	}
	if (Current->ep_square) Next->key ^= EPKey[File(Current->ep_square)];
	Next->turn = Current->turn ^ 1;
	Next->key ^= TurnKey;
	Entry = Hash + (High32(Next->key) & hash_mask);
	_mm_prefetch((char *)Entry,_MM_HINT_NTA);
	occ = Board->bb[White] | Board->bb[Black];
	if (piece >= WhiteLight) {
		if (piece < WhiteRook) Next->attacks[to] = BishopAttacks(to,occ);
		else if (piece < WhiteQueen) Next->attacks[to] = RookAttacks(to,occ);
		else if (piece < WhiteKing) Next->attacks[to] = QueenAttacks(to,occ);
	}
	mask_to = ~mask_to;
	for (u = (Board->bb[WhiteLight] | Board->bb[BlackLight] | Board->bb[WhiteDark] | Board->bb[BlackDark]) & mask_to; True(u); Cut(u)) {
		from = lsb(u);
		if (False(Current->attacks[from] & mask_from)) Next->attacks[from] = Current->attacks[from];
		else Next->attacks[from] = BishopAttacks(from,occ);
	}
	for (u = (Board->bb[WhiteRook] | Board->bb[BlackRook]) & mask_to; True(u); Cut(u)) {
		from = lsb(u);
		if (False(Current->attacks[from] & mask_from)) Next->attacks[from] = Current->attacks[from];
		else Next->attacks[from] = RookAttacks(from,occ);
	}
	for (u = (Board->bb[WhiteQueen] | Board->bb[BlackQueen]) & mask_to; True(u); Cut(u)) {
		from = lsb(u);
		if (False(Current->attacks[from] & mask_from)) Next->attacks[from] = Current->attacks[from];
		else {
			if (RMask[from] & mask_from) Next->attacks[from] = RookAttacks(from,occ) | (Current->attacks[from] & BMask[from]);
			else Next->attacks[from] = BishopAttacks(from,occ) | (Current->attacks[from] & RMask[from]);
		}
	}
}

void do_move(int move) {
	int from, to, piece, capture, me;
	GData * Next;
	uint64 u, updated, mask_from, mask_to, occ;
	to = To(move);
	Next = Current + 1;
	Next->ep_square = 0;
	capture = Board->square[to];
	if (capture) {
		do_capture(move,to,capture,Next);
		Next->ply = 0;
		goto finish;
	} else Next->capture = 0;
	Next->ply = Current->ply + 1;
	from = From(move);
	piece = Board->square[from];
	Board->square[from] = 0;
	Board->square[to] = piece;
	Next->piece = piece;
	mask_from = Bit(from);
	mask_to = Bit(to);
	me = piece & 1;
	Board->bb[piece] ^= mask_from;
	Board->bb[me] ^= mask_from;
	Board->bb[piece] |= mask_to;
	Board->bb[me] |= mask_to;
	updated = mask_from | mask_to;
	Next->flags = Current->flags & UpdateCastling[to] & UpdateCastling[from];
	Next->pst = Current->pst + PST[piece][to] - PST[piece][from];
	Next->key = Current->key ^ PieceKey[piece][to] ^ PieceKey[piece][from];
	Next->material = Current->material;
	if (piece < WhiteKnight) {
		Next->ply = 0;
		Next->pawn_key = Current->pawn_key ^ PieceKey[piece][to] ^ PieceKey[piece][from];
		if (IsEP(move)) {
			Board->square[to ^ 8] = 0;
			u = Bit(to ^ 8);
			updated |= u;
			Next->key ^= PieceKey[BlackPawn ^ me][to ^ 8];
			Next->pawn_key ^= PieceKey[BlackPawn ^ me][to ^ 8];
			Next->pst -= PST[BlackPawn ^ me][to ^ 8];
			Board->bb[BlackPawn ^ me] &= ~u;
			Board->bb[1 ^ me] &= ~u;
			Next->material -= MatCode[BlackPawn ^ me];
		} else if (IsPromotion(move)) {
			piece = Promotion(move,me);
			Board->square[to] = piece;
		    Next->material += MatCode[piece] - MatCode[WhitePawn | me];
			if (piece < WhiteRook) {
				if (piece >= WhiteLight && True(Board->bb[piece])) Next->material |= FlagUnusualMaterial;
				if (Multiple(Board->bb[piece])) Next->material |= FlagUnusualMaterial;
			} else if (Multiple(Board->bb[piece])) Next->material |= FlagUnusualMaterial;
			Board->bb[WhitePawn | me] ^= mask_to;
			Board->bb[piece] |= mask_to;
			Next->pst += PST[piece][to] - PST[WhitePawn | me][to];
			Next->key ^= PieceKey[piece][to] ^ PieceKey[WhitePawn | me][to];
			Next->pawn_key ^= PieceKey[WhitePawn | me][to];
		} else if (Eq(to ^ from, 16)) {
			if (PAtt[me][(to + from) >> 1] & Board->bb[BlackPawn ^ me]) {
				Next->ep_square = (to + from) >> 1;
				Next->key ^= EPKey[File(Next->ep_square)];
			}
		}
		PawnEntry = PawnHash + (High32(Next->pawn_key) & pawn_hash_mask);
	    _mm_prefetch((char *)PawnEntry,_MM_HINT_NTA);
	} else {
		if (piece < WhiteKing) Next->pawn_key = Current->pawn_key;
		else {
			Next->pawn_key = Current->pawn_key ^ PieceKey[piece][to] ^ PieceKey[piece][from];
			PawnEntry = PawnHash + (High32(Next->pawn_key) & pawn_hash_mask);
	        _mm_prefetch((char *)PawnEntry,_MM_HINT_NTA);
		}
		if (IsCastling(move)) {
			int rold, rnew;
			Next->ply = 0;
			if (to == 6) {
			    rold = 7; 
			    rnew = 5;
		    } else if (to == 2) {
                rold = 0; 
			    rnew = 3;
		    } else if (to == 62) {
                rold = 63;
			    rnew = 61;
		    } else if (to == 58) {
                rold = 56; 
			    rnew = 59;
		    }
			Add(mask_to,rnew);
			updated |= Bit(rnew) | Bit(rold);
			Board->square[rold] = 0;
			Board->square[rnew] = WhiteRook | me;
			Board->bb[WhiteRook | me] ^= Bit(rold);
			Board->bb[me] ^= Bit(rold);
			Board->bb[WhiteRook | me] |= Bit(rnew);
			Board->bb[me] |= Bit(rnew);
			Next->pst += PST[WhiteRook | me][rnew] - PST[WhiteRook | me][rold];
			Next->key ^= PieceKey[WhiteRook | me][rnew] ^ PieceKey[WhiteRook | me][rold];
			Next->attacks[rnew] = RookAttacks(rnew,Board->bb[White] | Board->bb[Black]);
		}
	}

	if (Current->ep_square) Next->key ^= EPKey[File(Current->ep_square)];
	Next->turn = Current->turn ^ 1;
	Next->key ^= TurnKey;
	Entry = Hash + (High32(Next->key) & hash_mask);
	_mm_prefetch((char *)Entry,_MM_HINT_NTA);

	mask_to = ~mask_to;
	occ = Board->bb[White] | Board->bb[Black];
	if (piece >= WhiteLight) {
		if (piece < WhiteRook) Next->attacks[to] = BishopAttacks(to,occ);
		else if (piece < WhiteQueen) Next->attacks[to] = RookAttacks(to,occ);
		else if (piece < WhiteKing) Next->attacks[to] = QueenAttacks(to,occ);
	}
	for (u = (Board->bb[WhiteLight] | Board->bb[BlackLight] | Board->bb[WhiteDark] | Board->bb[BlackDark]) & mask_to; True(u); Cut(u)) {
		from = lsb(u);
		if (False(Current->attacks[from] & updated)) Next->attacks[from] = Current->attacks[from];
		else Next->attacks[from] = BishopAttacks(from,occ);
	}
	for (u = (Board->bb[WhiteRook] | Board->bb[BlackRook]) & mask_to; True(u); Cut(u)) {
		from = lsb(u);
		if (False(Current->attacks[from] & updated)) Next->attacks[from] = Current->attacks[from];
		else Next->attacks[from] = RookAttacks(from,occ);
	}
	for (u = (Board->bb[WhiteQueen] | Board->bb[BlackQueen]) & mask_to; True(u); Cut(u)) {
		from = lsb(u);
		if (False(Current->attacks[from] & updated)) Next->attacks[from] = Current->attacks[from];
		else {
			if (False(BMask[from] & updated)) Next->attacks[from] = RookAttacks(from,occ) | (Current->attacks[from] & BMask[from]);
			else if (False(RMask[from] & updated)) Next->attacks[from] = BishopAttacks(from,occ) | (Current->attacks[from] & RMask[from]);
			else Next->attacks[from] = QueenAttacks(from,occ);
		}
	}
finish:
	sp++;
	Stack[sp] = Next->key;
	Next->move = move;
	Next->gen_flags = 0;
	Current++;
	nodes++;
}

void undo_move(int move) {
	int to, from, opp, piece;

	from = From(move);
	to = To(move);
	opp = Current->turn;
	if (IsPromotion(move)) {
		Board->bb[Board->square[to]] ^= Bit(to);
		piece = BlackPawn ^ opp;
	} else piece = Board->square[to];
	Board->square[from] = piece;
	Board->bb[piece] |= Bit(from);
	Board->bb[1 ^ opp] |= Bit(from);
	Board->bb[piece] &= ~Bit(to);
	Board->bb[1 ^ opp] ^= Bit(to);
	Board->square[to] = Current->capture;
	if (Current->capture) {
	    Board->bb[Current->capture] |= Bit(to);
	    Board->bb[opp] |= Bit(to);
	} else {
		if (IsCastling(move)) {
			int rold, rnew;
			if (to == 6) {
			    rold = 7; 
			    rnew = 5;
		    } else if (to == 2) {
                rold = 0; 
			    rnew = 3;
		    } else if (to == 62) {
                rold = 63;
			    rnew = 61;
		    } else if (to == 58) {
                rold = 56; 
			    rnew = 59;
		    }
			Board->square[rnew] = 0;
			Board->square[rold] = BlackRook ^ opp;
			Board->bb[BlackRook ^ opp] ^= Bit(rnew);
			Board->bb[1 ^ opp] ^= Bit(rnew);
			Board->bb[BlackRook ^ opp] |= Bit(rold);
			Board->bb[1 ^ opp] |= Bit(rold);
		} else if (IsEP(move)) {
			to = to ^ 8;
			piece = WhitePawn | opp;
			Board->square[to] = piece;
			Board->bb[opp] |= Bit(to);
			Board->bb[WhitePawn | opp] |= Bit(to);
		}
	}
	Current--;
	sp--;
}

inline void do_null() {
	int sq;
	uint64 u;
	GData * Next;

	Next = Current + 1;
	Next->key = Current->key ^ TurnKey;
	Entry = Hash + (High32(Next->key) & hash_mask);
	_mm_prefetch((char *)Entry,_MM_HINT_NTA);
	Next->pawn_key = Current->pawn_key;
	Next->eval_key = 0;
	Next->turn = Current->turn ^ 1;
	Next->material = Current->material;
	Next->pst = Current->pst;
	Next->ply = 0;
	Next->flags = Current->flags;
	Next->ep_square = 0;
	Next->capture = 0;
	if (Current->ep_square) Next->key ^= EPKey[File(Current->ep_square)];
	for (u = ((Board->bb[WhiteLight] | Board->bb[BlackLight]) | (Board->bb[WhiteDark] | Board->bb[BlackDark])) 
	| ((Board->bb[WhiteRook] | Board->bb[BlackRook]) | (Board->bb[WhiteQueen] | Board->bb[BlackQueen])); True(u); Cut(u)) {
		sq = lsb(u);
		Next->attacks[sq] = Current->attacks[sq];
	}
	Next->all_att[White] = Current->all_att[White];
	Next->all_att[Black] = Current->all_att[Black];
	// evaluation copy is not precise
	Next->score = -Current->score;
	Next->move = 0;
	Next->gen_flags = 0;
	Next->passer = Current->passer;
	Next->pawn_att[White] = Current->pawn_att[White];
	Next->pawn_att[Black] = Current->pawn_att[Black];
	sp++;	
	Stack[sp] = Next->key;
	Current++;
	nodes++;
}

inline void undo_null() {
	Current--;
	sp--;
}

int * gen_captures(uint64 target, int * list) {
	uint64 u, v;
	int from, to;

	if (Current->turn == White) {
	    if (Current->ep_square) {
		    if ((((Board->bb[WhitePawn]) & 0x00FEFEFEFEFEFEFE) << 7) & Bit(Current->ep_square)) {
		    	*list = Combine(((Current->ep_square - 7) << 6) | Current->ep_square | FlagEP,17); list++;
		    }
		    if ((((Board->bb[WhitePawn]) & 0xFFFF7F7F7F7F7F7F) << 9) & Bit(Current->ep_square)) {
	    		*list = Combine(((Current->ep_square - 9) << 6) | Current->ep_square | FlagEP,17); list++;
	    	}
	    }
		if (Current->all_att[White] & target) {
	    	for (u = (((Board->bb[WhitePawn]) & 0x0000FEFEFEFEFE00) << 7) & target; True(u); Cut(u)) {
                to = lsb(u);
    			*list = Combine(((to - 7) << 6) | to,((Board->square[to]) << 2) + 5); list++;
	    	}
    		for (u = (((Board->bb[WhitePawn]) & 0x00007F7F7F7F7F00) << 9) & target; True(u); Cut(u)) {
                to = lsb(u);
	    		*list = Combine(((to - 9) << 6) | to,((Board->square[to]) << 2) + 5); list++;
		    }
		    for (u = Board->bb[WhiteKnight]; True(u); Cut(u)) {
			    from = lsb(u);
			    for (v = NAtt[from] & target; True(v); Cut(v)) {
				    to = lsb(v);
				    *list = Combine((from << 6) | to,((Board->square[to]) << 2) + 4); list++;
			    }
		    }
		    for (u = Board->bb[WhiteLight] | Board->bb[WhiteDark]; True(u); Cut(u)) {
			    from = lsb(u);
			    for (v = Current->attacks[from] & target; True(v); Cut(v)) {
			    	to = lsb(v);
				    *list = Combine((from << 6) | to,((Board->square[to]) << 2) + 3); list++;
		    	}
		    }
		    from = lsb(Board->bb[WhiteKing]);
		    for (v = SArea[from] & target & (~Current->all_att[Black]); True(v); Cut(v)) {
	    		to = lsb(v);
	    		*list = Combine((from << 6) | to,((Board->square[to]) << 2) + 2); list++;
	    	}
	    	for (u = Board->bb[WhiteRook]; True(u); Cut(u)) {
	    		from = lsb(u);
	    		for (v = Current->attacks[from] & target; True(v); Cut(v)) {
    				to = lsb(v);
    				*list = Combine((from << 6) | to,((Board->square[to]) << 2) + 1); list++;
    			}
    		}
    		for (u = Board->bb[WhiteQueen]; True(u); Cut(u)) {
    			from = lsb(u);
    			for (v = Current->attacks[from] & target; True(v); Cut(v)) {
    				to = lsb(v);
    				*list = Combine((from << 6) | to,(Board->square[to]) << 2); list++;
    			}
    		}
    	}
	    for (u = (Board->bb[WhitePawn]) & Line[6]; True(u); Cut(u)) {
            from = lsb(u);
    		to = from + 8;
    		if (False(Board->square[to])) {
                *list = Combine((from << 6) | to | FlagPQueen,11); list++;
    		}
    		to = from + 7;
    		if (from != 48 && Odd(Board->square[to])) {
                *list = Combine((from << 6) | to | FlagPQueen,((Board->square[to]) << 2) + 6); list++;
    		}
    		to = from + 9;
    		if (from != 55 && (Board->square[to] & 1) != 0) {
                *list = Combine((from << 6) | to | FlagPQueen,((Board->square[to]) << 2) + 6); list++;
    		}
	    }
    } else {
	    if (Current->ep_square) {
    		if ((((Board->bb[BlackPawn]) >> 9) & 0x007F7F7F7F7F7F7F) & Bit(Current->ep_square)) {
	    		*list = Combine(((Current->ep_square + 9) << 6) | Current->ep_square | FlagEP,17); list++;
	    	}
    		if ((((Board->bb[BlackPawn]) >> 7) & 0x00FEFEFEFEFEFEFE) & Bit(Current->ep_square)) {
    			*list = Combine(((Current->ep_square + 7) << 6) | Current->ep_square | FlagEP,17); list++;
    		}
    	}
    	if (Current->all_att[Black] & target) {
	    	for (u = (((Board->bb[BlackPawn]) >> 9) & target) & 0x00007F7F7F7F7F00; True(u); Cut(u)) {
                to = lsb(u);
    			*list = Combine(((to + 9) << 6) | to,((Board->square[to]) << 2) + 9); list++;
    		}
    		for (u = (((Board->bb[BlackPawn]) >> 7) & target) & 0x0000FEFEFEFEFE00; True(u); Cut(u)) {
                to = lsb(u);
	    		*list = Combine(((to + 7) << 6) | to,((Board->square[to]) << 2) + 9); list++;
	    	}
	    	for (u = Board->bb[BlackKnight]; True(u); Cut(u)) {
	    		from = lsb(u);
	    		for (v = NAtt[from] & target; True(v); Cut(v)) {
    				to = lsb(v);
	    			*list = Combine((from << 6) | to,((Board->square[to]) << 2) + 8); list++;
	    		}
    		}
	    	for (u = Board->bb[BlackLight] | Board->bb[BlackDark]; True(u); Cut(u)) {
	    		from = lsb(u);
	    		for (v = Current->attacks[from] & target; True(v); Cut(v)) {
	    			to = lsb(v);
	    			*list = Combine((from << 6) | to,((Board->square[to]) << 2) + 7); list++;
	    		}
	    	}
	    	from = lsb(Board->bb[BlackKing]);
	    	for (v = SArea[from] & target & (~Current->all_att[White]); True(v); Cut(v)) {
	    		to = lsb(v);
	    		*list = Combine((from << 6) | to,((Board->square[to]) << 2) + 6); list++;
	    	}
	    	for (u = Board->bb[BlackRook]; True(u); Cut(u)) {
	    		from = lsb(u);
	    		for (v = Current->attacks[from] & target; True(v); Cut(v)) {
	    			to = lsb(v);
	    			*list = Combine((from << 6) | to,((Board->square[to]) << 2) + 5); list++;
	    		}
	    	}
	    	for (u = Board->bb[BlackQueen]; True(u); Cut(u)) {
	    		from = lsb(u);
	    		for (v = Current->attacks[from] & target; True(v); Cut(v)) {
	    			to = lsb(v);
	    			*list = Combine((from << 6) | to,((Board->square[to]) << 2) + 4); list++;
	    		}
    		}
    	}
	    for (u = (Board->bb[BlackPawn]) & Line[1]; True(u); Cut(u)) {
            from = lsb(u);
	    	to = from - 8;
	    	if (False(Board->square[to])) {
                *list = Combine((from << 6) | to | FlagPQueen,11); list++;
	    	}
	    	to = from - 7;
	    	if (from != 15 && True(Board->square[to]) && Even(Board->square[to])) {
                *list = Combine((from << 6) | to | FlagPQueen,((Board->square[to]) << 2) + 10); list++;
	    	}
		    to = from - 9;
		    if (from != 8 && True(Board->square[to]) && Even(Board->square[to])) {
                *list = Combine((from << 6) | to | FlagPQueen,((Board->square[to]) << 2) + 10); list++;
	    	}
	    }
	}
	*list = 0;
	return list;
}

int * gen_quiet_moves(int * list) {
	int to, from;
	uint64 u, v, free, occ;

    occ = Board->bb[White] | Board->bb[Black];
	free = ~occ;
	if (Current->turn == White) {
	    if (True(Current->flags & CanCastle_OO) && False((occ | Current->all_att[Black]) & 0x60)) {
            *list = 0x106 | FlagCastling | History(WhiteKing,6); list++;
	    }
	    if (True(Current->flags & CanCastle_OOO) && (occ & 0x0E) == 0 && False(Current->all_att[Black] & 0x0C)) {
            *list = 0x102 | FlagCastling | History(WhiteKing,2); list++;
	    }

	    for (u = ((Board->bb[WhitePawn] & 0x0000FFFFFFFFFF00) << 8) & free; True(u); Cut(u)) {
            from = lsb(u);
			if (((from & 0xFFFFFFF8) == 16) && False(Board->square[from + 8])) {
                *list = ((from - 8) << 6) | (from + 8) | History(WhitePawn,from + 8); list++;
			}
			*list = ((from - 8) << 6) | from | History(WhitePawn,from); list++;
		}
		for (u = Board->bb[WhiteKnight]; True(u); Cut(u)) {
            from = lsb(u);
			for (v = free & NAtt[from]; True(v); Cut(v)) {
                to = lsb(v);
				*list = (from << 6) | to | History(WhiteKnight,to); list++;
			}
		}
		for (u = (Board->bb[WhiteLight] | Board->bb[WhiteDark] | Board->bb[WhiteRook] | Board->bb[WhiteQueen]); True(u); Cut(u)) {
            from =lsb(u);
			for (v = free & Current->attacks[from]; True(v); Cut(v)) {
				to = lsb(v);
				*list = (from << 6) | to | History(Board->square[from],to); list++;
			}
		}
		from = lsb(Board->bb[WhiteKing]);
		for (u = SArea[from] & free & (~Current->all_att[Black]); True(u); Cut(u)) {
            to = lsb(u);
			*list = (from << 6) | to | History(WhiteKing,to); list++;
		}
	} else {
		if (True(Current->flags & CanCastle_oo) && False((occ | Current->all_att[White]) & 0x6000000000000000)) {
            *list = 0xF3E | FlagCastling | History(BlackKing,62); list++;
	    }
	    if (True(Current->flags & CanCastle_ooo) && (occ & 0x0E00000000000000) == 0 && False(Current->all_att[White] & 0x0C00000000000000)) {
            *list = 0xF3A | FlagCastling | History(BlackKing,58); list++;
	    }

	    for (u = ((Board->bb[BlackPawn] >> 8) & 0x0000FFFFFFFFFF00) & free; True(u); Cut(u)) {
            from = lsb(u);
			if (((from & 0xFFFFFFF8) == 40) && False(Board->square[from - 8])) {
                *list = ((from + 8) << 6) | (from - 8) | History(BlackPawn,from - 8); list++;
			}
			*list = ((from + 8) << 6) | from | History(BlackPawn,from); list++;
		}
		for (u = Board->bb[BlackKnight]; True(u); Cut(u)) {
            from = lsb(u);
			for (v = free & NAtt[from]; True(v); Cut(v)) {
                to = lsb(v);
				*list = (from << 6) | to | History(BlackKnight,to); list++;
			}
		}
		for (u = (Board->bb[BlackLight] | Board->bb[BlackDark] | Board->bb[BlackRook] | Board->bb[BlackQueen]); True(u); Cut(u)) {
            from =lsb(u);
			for (v = free & Current->attacks[from]; True(v); Cut(v)) {
				to = lsb(v);
				*list = (from << 6) | to | History(Board->square[from],to); list++;
			}
		}
		from = lsb(Board->bb[BlackKing]);
		for (u = SArea[from] & free & (~Current->all_att[White]); True(u); Cut(u)) {
            to = lsb(u);
			*list = (from << 6) | to | History(BlackKing,to); list++;
		}
	}
	*list = 0;
	return list;
}

int * gen_delta_moves(int * list) {
	int to, from;
	uint64 u, v, free, occ;

    occ = Board->bb[White] | Board->bb[Black];
	free = ~occ;
	if (Current->turn == White) {
	    if (True(Current->flags & CanCastle_OO) && False((occ | Current->all_att[Black]) & 0x60) && DeltaMargin(WhiteKing,6)) {
            *list = 0x106 | FlagCastling | History(WhiteKing,6); list++;
	    }
	    if (True(Current->flags & CanCastle_OOO) && (occ & 0x0E) == 0 && False(Current->all_att[Black] & 0x0C) && DeltaMargin(WhiteKing,2)) {
            *list = 0x102 | FlagCastling | History(WhiteKing,2); list++;
	    }

	    for (u = ((Board->bb[WhitePawn] & 0x0000FFFFFFFFFF00) << 8) & free; True(u); Cut(u)) {
            from = lsb(u);
			if (((from & 0xFFFFFFF8) == 16) && False(Board->square[from + 8]) && DeltaMargin(WhitePawn,from+8)) {
                *list = ((from - 8) << 6) | (from + 8) | History(WhitePawn,from + 8); list++;
			}
			if (DeltaMargin(WhitePawn,from)) {
			    *list = ((from - 8) << 6) | from | History(WhitePawn,from); list++;
			}
		}
		for (u = Board->bb[WhiteKnight]; True(u); Cut(u)) {
            from = lsb(u);
			for (v = free & NAtt[from]; True(v); Cut(v)) {
                to = lsb(v);
				if (DeltaMargin(WhiteKnight,to)) {
					*list = (from << 6) | to | History(WhiteKnight,to); list++;
				}
			}
		}
		for (u = (Board->bb[WhiteLight] | Board->bb[WhiteDark] | Board->bb[WhiteRook] | Board->bb[WhiteQueen]); True(u); Cut(u)) {
            from =lsb(u);
			for (v = free & Current->attacks[from]; True(v); Cut(v)) {
				to = lsb(v);
				if (DeltaMargin(Board->square[from],to)) {
					*list = (from << 6) | to | History(Board->square[from],to); list++;
				}
			}
		}
		from = lsb(Board->bb[WhiteKing]);
		for (u = SArea[from] & free & (~Current->all_att[Black]); True(u); Cut(u)) {
            to = lsb(u);
			if (DeltaMargin(WhiteKing,to)) {
				*list = (from << 6) | to | History(WhiteKing,to); list++;
			}
		}
	} else {
		if (True(Current->flags & CanCastle_oo) && False((occ | Current->all_att[White]) & 0x6000000000000000) && DeltaMargin(BlackKing,62)) {
            *list = 0xF3E | FlagCastling | History(BlackKing,62); list++;
	    }
	    if (True(Current->flags & CanCastle_ooo) && (occ & 0x0E00000000000000) == 0 && False(Current->all_att[White] & 0x0C00000000000000) && DeltaMargin(BlackKing,58)) {
            *list = 0xF3A | FlagCastling | History(BlackKing,58); list++;
	    }

	    for (u = ((Board->bb[BlackPawn] >> 8) & 0x0000FFFFFFFFFF00) & free; True(u); Cut(u)) {
            from = lsb(u);
			if (((from & 0xFFFFFFF8) == 40) && False(Board->square[from - 8]) && DeltaMargin(BlackPawn,from-8)) {
                *list = ((from + 8) << 6) | (from - 8) | History(BlackPawn,from - 8); list++;
			}
			if (DeltaMargin(BlackPawn,from)) {
			    *list = ((from + 8) << 6) | from | History(BlackPawn,from); list++;
			}
		}
		for (u = Board->bb[BlackKnight]; True(u); Cut(u)) {
            from = lsb(u);
			for (v = free & NAtt[from]; True(v); Cut(v)) {
                to = lsb(v);
				if (DeltaMargin(BlackKnight,to)) {
				    *list = (from << 6) | to | History(BlackKnight,to); list++;
				}
			}
		}
		for (u = (Board->bb[BlackLight] | Board->bb[BlackDark] | Board->bb[BlackRook] | Board->bb[BlackQueen]); True(u); Cut(u)) {
            from =lsb(u);
			for (v = free & Current->attacks[from]; True(v); Cut(v)) {
				to = lsb(v);
				if (DeltaMargin(Board->square[from],to)) {
				    *list = (from << 6) | to | History(Board->square[from],to); list++;
				}
			}
		}
		from = lsb(Board->bb[BlackKing]);
		for (u = SArea[from] & free & (~Current->all_att[White]); True(u); Cut(u)) {
            to = lsb(u);
			if (DeltaMargin(BlackKing,to)) {
			    *list = (from << 6) | to | History(BlackKing,to); list++;
			}
		}
	}
	*list = 0;
	return list;
}

int * gen_evasions(int * list) {
	int king, att_sq, from;
	uint64 att, esc, u;

	if (Current->turn == White) {
		king = lsb(Board->bb[WhiteKing]);
		att = (NAtt[king] & Board->bb[BlackKnight]) | (PAtt[White][king] & Board->bb[BlackPawn]);
		for (u = (BMask[king] & (Board->bb[BlackLight] | Board->bb[BlackDark] | Board->bb[BlackQueen])) | 
			(RMask[king] & (Board->bb[BlackRook] | Board->bb[BlackQueen])); True(u); Cut(u)) {
			from = lsb(u);
			if (Current->attacks[from] & Board->bb[WhiteKing]) Add(att,from);
		}
		att_sq = lsb(att);
		esc = SArea[king] & (~Board->bb[White]) & (~Current->all_att[Black]);
		if (Board->square[att_sq] >= WhiteLight) esc &= ~FullLine[king][att_sq];
		Cut(att);
		if (att) {
			att_sq = lsb(att);
			if (Board->square[att_sq] >= WhiteLight) esc &= ~FullLine[king][att_sq];
			while (esc) {
				*list = Combine((king << 6) | lsb(esc),((Board->square[lsb(esc)]) << 2) + 2); list++;
			    Cut(esc);
			}
			*list = 0;
			return list;
		}
		if (Current->ep_square == att_sq + 8) {
			for (u = PAtt[Black][att_sq + 8] & Board->bb[WhitePawn]; True(u); Cut(u)) {
			    *list = Combine((lsb(u) << 6) | (att_sq + 8) | FlagEP,17); list++;
			}
		}
		for (u = PAtt[Black][att_sq] & Board->bb[WhitePawn]; True(u); Cut(u)) {
            from = lsb(u);
			*list = Combine((from << 6) | att_sq,((Board->square[att_sq]) << 2) + 5);
			if (att_sq >= 56) *list |= FlagPQueen;
			list++;
		}
		while (esc) {
            *list = Combine((king << 6) | lsb(esc),((Board->square[lsb(esc)]) << 2) + 2); list++;
			Cut(esc);
		}
		att = Between[king][att_sq];
		for (u = (att >> 8) & Board->bb[WhitePawn]; True(u); Cut(u)) {
            from = lsb(u);
			*list = Combine((from << 6) | (from + 8),5);
			if (from >= 48) *list |= FlagPQueen;
			list++;
		}
		for (u = Line[1] & ((~(Board->bb[White] | Board->bb[Black])) >> 8) & (att >> 16) & Board->bb[WhitePawn]; True(u); Cut(u)) {
            *list = Combine((lsb(u) << 6) | (lsb(u) + 16),5); list++;
		}
		att |= Bit(att_sq);
		for (u = Board->bb[WhiteKnight]; True(u); Cut(u)) {
            from = lsb(u);
			for (esc = NAtt[from] & att; True(esc); Cut(esc)) {
				*list = Combine((from << 6) | lsb(esc),((Board->square[lsb(esc)]) << 2) + 4); list++;
			}
		}
		for (u = (Board->bb[WhiteLight] | Board->bb[WhiteDark] | Board->bb[WhiteRook] | Board->bb[WhiteQueen]); True(u); Cut(u)) {
            from = lsb(u);
			for (esc = Current->attacks[from] & att; True(esc); Cut(esc)) {
				*list = Combine((from << 6) | lsb(esc),((Board->square[lsb(esc)]) << 2) + 6 - (Board->square[from] >> 1)); list++;
			}
		}
	} else {
		king = lsb(Board->bb[BlackKing]);
		att = (NAtt[king] & Board->bb[WhiteKnight]) | (PAtt[Black][king] & Board->bb[WhitePawn]);
		for (u = (BMask[king] & (Board->bb[WhiteLight] | Board->bb[WhiteDark] | Board->bb[WhiteQueen])) | 
			(RMask[king] & (Board->bb[WhiteRook] | Board->bb[WhiteQueen])); True(u); Cut(u)) {
			from = lsb(u);
			if (Current->attacks[from] & Board->bb[BlackKing]) Add(att,from);
		}
		att_sq = lsb(att);
		esc = SArea[king] & (~Board->bb[Black]) & (~Current->all_att[White]);
		if (Board->square[att_sq] >= WhiteLight) esc &= ~FullLine[king][att_sq];
		Cut(att);
		if (att) {
			att_sq = lsb(att);
			if (Board->square[att_sq] >= WhiteLight) esc &= ~FullLine[king][att_sq];
			while (esc) {
				*list = Combine((king << 6) | lsb(esc),((Board->square[lsb(esc)]) << 2) + 6); list++;
			    Cut(esc);
			}
			*list = 0;
			return list;
		}
		if (Current->ep_square > 0 && Current->ep_square == att_sq - 8) {
			for (u = PAtt[White][att_sq - 8] & Board->bb[BlackPawn]; True(u); Cut(u)) {
			    *list = Combine((lsb(u) << 6) | (att_sq - 8) | FlagEP,17); list++;
			}
		}
		for (u = PAtt[White][att_sq] & Board->bb[BlackPawn]; True(u); Cut(u)) {
            from = lsb(u);
			*list = Combine((from << 6) | att_sq,((Board->square[att_sq]) << 2) + 9);
			if (att_sq < 8) *list |= FlagPQueen;
			list++;
		}
		while (esc) {
            *list = Combine((king << 6) | lsb(esc),((Board->square[lsb(esc)]) << 2) + 6); list++;
			Cut(esc);
		}
		att = Between[king][att_sq];
		for (u = (att << 8) & Board->bb[BlackPawn]; True(u); Cut(u)) {
            from = lsb(u);
			*list = Combine((from << 6) | (from - 8),9);
			if (from < 16) *list |= FlagPQueen;
			list++;
		}
		for (u = Line[6] & ((~(Board->bb[White] | Board->bb[Black])) << 8) & (att << 16) & Board->bb[BlackPawn]; True(u); Cut(u)) {
            *list = Combine((lsb(u) << 6) | (lsb(u) - 16),9); list++;
		}
		att |= Bit(att_sq);
		for (u = Board->bb[BlackKnight]; True(u); Cut(u)) {
            from = lsb(u);
			for (esc = NAtt[from] & att; True(esc); Cut(esc)) {
				*list = Combine((from << 6) | lsb(esc),((Board->square[lsb(esc)]) << 2) + 8); list++;
			}
		}
		for (u = (Board->bb[BlackLight] | Board->bb[BlackDark]| Board->bb[BlackRook] | Board->bb[BlackQueen]); True(u); Cut(u)) {
            from = lsb(u);
			for (esc = Current->attacks[from] & att; True(esc); Cut(esc)) {
				*list = Combine((from << 6) | lsb(esc),((Board->square[lsb(esc)]) << 2) + 10 - (Board->square[from] >> 1)); list++;
			}
		}
	}
	*list = 0;
	return list;
}

int * gen_checks(uint64 capture_target, int * list) { // this function doesn't generate ALL checks
	int king, from;
    uint64 u, v, occ, target, b_target, r_target, clear, xray;

	occ = Board->bb[White] | Board->bb[Black];
	clear = (~occ) | capture_target;
	if (Current->turn == White) {
		king = lsb(Board->bb[BlackKing]);
		b_target = BishopAttacks(king,occ);
		r_target = RookAttacks(king,occ);
		xray = 0;
		for (u = b_target & ((Board->bb[WhitePawn] & 0x0000FFFFFFFFFF00) | Board->bb[WhiteKnight] | Board->bb[WhiteRook]); True(u); u ^= target) {
			from = lsb(u);
			target = Bit(from);
			for (v = FullLine[king][from] & (Board->bb[WhiteLight] | Board->bb[WhiteDark] | Board->bb[WhiteQueen]); True(v); Cut(v)) {
				if (Current->attacks[lsb(v)] & target) {
					if (target & Board->bb[WhitePawn]) {
						if (Board->square[from + 8] == 0) {
							*list = Combine((from << 6) | (from + 8),110); list++;
						}
						v = PAtt[White][from] & capture_target & (~FullLine[king][from]);
						if (v) {
						    *list = Combine((from << 6) | lsb(v),((Board->square[lsb(v)]) << 2) + 105); list++;
						}
					} else if (target & Board->bb[WhiteKnight]) {
						for (v = NAtt[from] & clear; True(v); Cut(v)) {
							*list = Combine((from << 6) | lsb(v),((Board->square[lsb(v)]) << 2) + 104); list++;
						}
					} else {
						for (v = Current->attacks[from] & clear; True(v); Cut(v)) {
							*list = Combine((from << 6) | lsb(v),((Board->square[lsb(v)]) << 2) + 101); list++;
						}
					}
					xray |= target;
					break;
				}
			}
		}
		for (u = r_target & ((Board->bb[WhitePawn] & 0x0000FFFFFFFFFF00) | Board->bb[WhiteKnight] | Board->bb[WhiteLight] | Board->bb[WhiteDark]); True(u); u ^= target) {
			from = lsb(u);
			target = Bit(from);
			for (v = FullLine[king][from] & (Board->bb[WhiteRook] | Board->bb[WhiteQueen]); True(v); Cut(v)) {
				if (Current->attacks[lsb(v)] & target) {
					if (target & Board->bb[WhitePawn]) {
						if (Board->square[from + 8] == 0 && Rank(from) == Rank(king)) {
							*list = Combine((from << 6) | (from + 8),110); list++;
						}
						for (v = PAtt[White][from] & capture_target; True(v); Cut(v)) {
						    *list = Combine((from << 6) | lsb(v),((Board->square[lsb(v)]) << 2) + 105); list++;
						}
					} else if (target & Board->bb[WhiteKnight]) {
						for (v = NAtt[from] & clear; True(v); Cut(v)) {
							*list = Combine((from << 6) | lsb(v),((Board->square[lsb(v)]) << 2) + 104); list++;
						}
					} else {
						for (v = Current->attacks[from] & clear; True(v); Cut(v)) {
							*list = Combine((from << 6) | lsb(v),((Board->square[lsb(v)]) << 2) + 103); list++;
						}
					}
					xray |= target;
					break;
				}
			}
		}
		if (Current->gen_flags & FlagSkipBadChecks) clear &= ~Current->pawn_att[Black];
		Current->xray = xray;
		xray = ~xray;
		for (u = Board->bb[WhiteKnight] & NArea[king] & xray; True(u); Cut(u)) {
            from = lsb(u);
			for (v = NAtt[king] & NAtt[from] & clear; True(v); Cut(v)) {
			    *list = Combine((from << 6) | lsb(v),((Board->square[lsb(v)]) << 2) + 4); list++;
			}
		}
        for (u = DArea[king] & Board->bb[WhitePawn] & 0x0000FFFFFFFFFF00 & xray; True(u); Cut(u)) {
			from = lsb(u);
			for (v = PAtt[White][from] & PAtt[Black][king] & capture_target; True(v); Cut(v)) {
				*list = Combine((from << 6) | lsb(v),((Board->square[lsb(v)]) << 2) + 5); list++;
			}
			if (Board->square[from + 8] == 0 && True(PAtt[White][from + 8] & Board->bb[BlackKing])) {
				*list = Combine((from << 6) | (from + 8),10); list++;
			}
		}
		b_target &= clear;
		for (u = (Board->bb[WhiteQueen] | (Odd(king ^ Rank(king)) ? Board->bb[WhiteLight] : Board->bb[WhiteDark])) & xray; True(u); Cut(u)) {
		    from = lsb(u);
			for (v = Current->attacks[from] & b_target; True(v); Cut(v)) {
				*list = Combine((from << 6) | lsb(v),((Board->square[lsb(v)]) << 2) + 6 - (Board->square[from] >> 1)); list++;
			}
		}
		r_target &= clear;
		for (u = (Board->bb[WhiteQueen] | Board->bb[WhiteRook]) & xray; True(u); Cut(u)) {
		    from = lsb(u);
			for (v = Current->attacks[from] & r_target; True(v); Cut(v)) {
				*list = Combine((from << 6) | lsb(v),((Board->square[lsb(v)]) << 2) + 6 - (Board->square[from] >> 1)); list++;
			}
		}
	} else {
		king = lsb(Board->bb[WhiteKing]);
		b_target = BishopAttacks(king,occ);
		r_target = RookAttacks(king,occ);
		xray = 0;
		for (u = b_target & ((Board->bb[BlackPawn] & 0x00FFFFFFFFFF0000) | Board->bb[BlackKnight] | Board->bb[BlackRook]); True(u); u ^= target) {
			from = lsb(u);
			target = Bit(from);
			for (v = FullLine[king][from] & (Board->bb[BlackLight] | Board->bb[BlackDark] | Board->bb[BlackQueen]); True(v); Cut(v)) {
				if (Current->attacks[lsb(v)] & target) {
					if (target & Board->bb[BlackPawn]) {
						if (Board->square[from - 8] == 0) {
							*list = Combine((from << 6) | (from - 8),110); list++;
						}
						v = PAtt[Black][from] & capture_target & (~FullLine[king][from]);
						if (v) {
						    *list = Combine((from << 6) | lsb(v),((Board->square[lsb(v)]) << 2) + 109); list++;
						}
					} else if (target & Board->bb[BlackKnight]) {
						for (v = NAtt[from] & clear; True(v); Cut(v)) {
							*list = Combine((from << 6) | lsb(v),((Board->square[lsb(v)]) << 2) + 108); list++;
						}
					} else {
						for (v = Current->attacks[from] & clear; True(v); Cut(v)) {
							*list = Combine((from << 6) | lsb(v),((Board->square[lsb(v)]) << 2) + 105); list++;
						}
					}
					xray |= target;
					break;
				}
			}
		}
		for (u = r_target & ((Board->bb[BlackPawn] & 0x00FFFFFFFFFF0000) | Board->bb[BlackKnight] | Board->bb[BlackLight] | Board->bb[BlackDark]); True(u); u ^= target) {
			from = lsb(u);
			target = Bit(from);
			for (v = FullLine[king][from] & (Board->bb[BlackRook] | Board->bb[BlackQueen]); True(v); Cut(v)) {
				if (Current->attacks[lsb(v)] & target) {
					if (target & Board->bb[BlackPawn]) {
						if (Board->square[from - 8] == 0 && Rank(from) == Rank(king)) {
							*list = Combine((from << 6) | (from - 8),110); list++;
						}
						for (v = PAtt[Black][from] & capture_target; True(v); Cut(v)) {
						    *list = Combine((from << 6) | lsb(v),((Board->square[lsb(v)]) << 2) + 109); list++;
						}
					} else if (target & Board->bb[BlackKnight]) {
						for (v = NAtt[from] & clear; True(v); Cut(v)) {
							*list = Combine((from << 6) | lsb(v),((Board->square[lsb(v)]) << 2) + 108); list++;
						}
					} else {
						for (v = Current->attacks[from] & clear; True(v); Cut(v)) {
							*list = Combine((from << 6) | lsb(v),((Board->square[lsb(v)]) << 2) + 107); list++;
						}
					}
					xray |= target;
					break;
				}
			}
		}
		if (Current->gen_flags & FlagSkipBadChecks) clear &= ~Current->pawn_att[White];
		Current->xray = xray;
		xray = ~xray;
		for (u = Board->bb[BlackKnight] & NArea[king] & xray; True(u); Cut(u)) {
            from = lsb(u);
			for (v = NAtt[king] & NAtt[from] & clear; True(v); Cut(v)) {
			    *list = Combine((from << 6) | lsb(v),((Board->square[lsb(v)]) << 2) + 9); list++;
			}
		}
        for (u = DArea[king] & Board->bb[BlackPawn] & 0x00FFFFFFFFFF0000 & xray; True(u); Cut(u)) {
			from = lsb(u);
			for (v = PAtt[Black][from] & PAtt[White][king] & capture_target; True(v); Cut(v)) {
				*list = Combine((from << 6) | lsb(v),((Board->square[lsb(v)]) << 2) + 10); list++;
			}
			if (Board->square[from - 8] == 0 && True(PAtt[Black][from - 8] & Board->bb[WhiteKing])) {
				*list = Combine((from << 6) | (from - 8),10); list++;
			}
		}
		b_target &= clear;
		for (u = (Board->bb[BlackQueen] | (Odd(king ^ Rank(king)) ? Board->bb[BlackLight] : Board->bb[BlackDark])) & xray; True(u); Cut(u)) {
		    from = lsb(u);
			for (v = Current->attacks[from] & b_target; True(v); Cut(v)) {
				*list = Combine((from << 6) | lsb(v),((Board->square[lsb(v)]) << 2) + 10 - (Board->square[from] >> 1)); list++;
			}
		}
		r_target &= clear;
		for (u = (Board->bb[BlackQueen] | Board->bb[BlackRook]) & xray; True(u); Cut(u)) {
		    from = lsb(u);
			for (v = Current->attacks[from] & r_target; True(v); Cut(v)) {
				*list = Combine((from << 6) | lsb(v),((Board->square[lsb(v)]) << 2) + 10 - (Board->square[from] >> 1)); list++;
			}
		}
	}
	*list = 0;
	return list;
}

int * gen_legal_moves() {
	int i, *p, *q, killer, depth = -256, move, value = -MateValue;

	killer = 0;
	for (i = 0, Entry = Hash + (High32(Current->key) & hash_mask); i < 4; i++, Entry++) {
		if (Low32(Current->key) == Entry->key) {
			if (True(Entry->move) && Entry->low_depth > depth) {
				depth = Entry->low_depth;
				killer = Entry->move;
				value = Entry->low;
			}
		}
	}
	for (i = 0, PVEntry = PVHash + (High32(Current->key) & pv_hash_mask); i < pv_cluster_size; i++, PVEntry++) {
		if (PVEntry->key == Low32(Current->key)) {
			if (PVEntry->depth >= depth && True(PVEntry->move)) {
				depth = PVEntry->depth;
				killer = PVEntry->move;
				value = PVEntry->value;
			}
			break;
		}
	}
	if (Current->all_att[Current->turn ^ 1] & Board->bb[WhiteKing | Current->turn]) gen_evasions(Current->moves);
	else {
	    p = gen_captures(Board->bb[Current->turn ^ 1],Current->moves);
	    gen_quiet_moves(p);
	}
	for (p = Current->moves, q = p; (move = ((*q) & 0xFFFF)) != 0; q++) {
		do_move(move);
		evaluate();
		if (False(Current->all_att[Current->turn] & Board->bb[BlackKing ^ Current->turn])) {
			if (SearchMoves) {
				for (i = 0; i < SMPointer; i++) {
					if (SMoves[i] == move) goto keep_move;
				}
				goto next_move;
			}
keep_move:
			*p = move;
			if (move != killer && True(Easy)) {
				if (-q_search(1 - (value - 150), 16, FlagHashCheck) >= value - 150) Easy = 0;
			}
			p++;
		}
next_move:
		undo_move(move);
	}
	*p = 0;
	for (q = Current->moves; q < p; q++) {
		if ((*q) == killer) {
			*q = Current->moves[0];
			best_move = Current->moves[0] = killer;
			break;
		}
	}
	return p;
}

int see(int move) {
	int from, to, piece, capture, me, delta, sq, pos;
	uint64 clear, def, att, occ, b_area, r_slider_att, b_slider_att, r_slider_def, b_slider_def, r_area, u, new_att, my_bishop, opp_bishop;

	from = From(move);
	to = To(move);
	piece = SEEValue[Board->square[from]];
	capture = SEEValue[Board->square[to]];
	delta = piece - capture;
	if (delta <= 0) return 1;
	if (IsEP(move)) return 1;
	me = Current->turn;
	if (PAtt[me][to] & Board->bb[BlackPawn ^ me]) return 0;
	clear = ~Bit(from);
	def = PAtt[me ^ 1][to] & Board->bb[WhitePawn | me] & clear;
	if (True(def) && delta <= 2) return 1;
	att = NAtt[to] & Board->bb[BlackKnight ^ me];
	if (True(att) && delta > 3) return 0;
	occ = (Board->bb[White] | Board->bb[Black]) & clear;
    b_area = BishopAttacks(to,occ);
	opp_bishop = Board->bb[BlackLight ^ me] | Board->bb[BlackDark ^ me];
	if (delta > 3) if (b_area & opp_bishop) return 0;
	my_bishop = Board->bb[WhiteLight | me] | Board->bb[WhiteDark | me];
    b_slider_att = BMask[to] & (opp_bishop | Board->bb[BlackQueen ^ me]);
	r_slider_att = RMask[to] & (Board->bb[BlackRook ^ me] | Board->bb[BlackQueen ^ me]);
	b_slider_def = BMask[to] & (my_bishop | Board->bb[WhiteQueen | me]) & clear;
	r_slider_def = RMask[to] & (Board->bb[WhiteRook | me] | Board->bb[WhiteQueen | me]) & clear;
	att |= (b_slider_att & b_area);
	def |= NAtt[to] & Board->bb[WhiteKnight | me] & clear;
	r_area = RookAttacks(to,occ);
	att |= (r_slider_att & r_area);
	def |= (b_slider_def & b_area);
	def |= (r_slider_def & r_area);
	att |= SArea[to] & Board->bb[BlackKing ^ me];
	def |= SArea[to] & Board->bb[WhiteKing | me] & clear;
	while (1) {
		if (u = (att & Board->bb[BlackKnight ^ me])) {
			capture -= piece;
			piece = 3;
			att ^= (~(u-1)) & u;
		} else if (u = (att & opp_bishop)) {
            capture -= piece;
			piece = 3;
			sq = lsb(u);
			occ ^= Bit(sq);
			att ^= Bit(sq);
			for (new_att = FullLine[to][sq] & b_slider_att & occ & (~att); True(new_att); Cut(new_att)) {
                pos = lsb(new_att);
				if (False(Between[to][pos] & occ)) {
                    Add(att,pos);
					break;
				}
			}
		} else if (u = (att & Board->bb[BlackRook ^ me])) {
            capture -= piece;
			piece = 5;
			sq = lsb(u);
			occ ^= Bit(sq);
			att ^= Bit(sq);
			for (new_att = FullLine[to][sq] & r_slider_att & occ & (~att); True(new_att); Cut(new_att)) {
                pos = lsb(new_att);
				if (False(Between[to][pos] & occ)) {
                    Add(att,pos);
					break;
				}
			}
		} else if (u = (att & Board->bb[BlackQueen ^ me])) {
            capture -= piece;
			piece = 10;
			sq = lsb(u);
			occ ^= Bit(sq);
			att ^= Bit(sq);
			for (new_att = FullLine[to][sq] & (r_slider_att | b_slider_att) & occ & (~att); True(new_att); Cut(new_att)) {
                pos = lsb(new_att);
				if (False(Between[to][pos] & occ)) {
                    Add(att,pos);
					break;
				}
			}
		} else if (u = (att & Board->bb[BlackKing ^ me])) {
            capture -= piece;
			piece = 1000;
		} else return 1;
		if (capture < -500) return 0;
		if (piece + capture < 0) return 0;
		if (u = (def & Board->bb[WhitePawn | me])) {
            capture += piece;
			piece = 1;
            sq = lsb(u);
			occ ^= Bit(sq);
			def ^= Bit(sq);
			for (new_att = FullLine[to][sq] & b_slider_def & occ & (~att); True(new_att); Cut(new_att)) {
                pos = lsb(new_att);
				if (False(Between[to][pos] & occ)) {
                    Add(def,pos);
					break;
				}
			}
		} else if (u = (def & Board->bb[WhiteKnight | me])) {
            capture += piece;
			piece = 3;
			def ^= (~(u-1)) & u;
		} else if (u = (def & my_bishop)) {
            capture += piece;
			piece = 3;
            sq = lsb(u);
			occ ^= Bit(sq);
			def ^= Bit(sq);
			for (new_att = FullLine[to][sq] & b_slider_def & occ & (~att); True(new_att); Cut(new_att)) {
                pos = lsb(new_att);
				if (False(Between[to][pos] & occ)) {
                    Add(def,pos);
					break;
				}
			}
		} else if (u = (def & Board->bb[WhiteRook | me])) {
            capture += piece;
			piece = 5;
            sq = lsb(u);
			occ ^= Bit(sq);
			def ^= Bit(sq);
			for (new_att = FullLine[to][sq] & r_slider_def & occ & (~att); True(new_att); Cut(new_att)) {
                pos = lsb(new_att);
				if (False(Between[to][pos] & occ)) {
                    Add(def,pos);
					break;
				}
			}
		} else if (u = (def & Board->bb[WhiteQueen | me])) {
            capture += piece;
			piece = 10;
			sq = lsb(u);
			occ ^= Bit(sq);
			def ^= Bit(sq);
			for (new_att = FullLine[to][sq] & (r_slider_def | b_slider_def) & occ & (~att); True(new_att); Cut(new_att)) {
                pos = lsb(new_att);
				if (False(Between[to][pos] & occ)) {
                    Add(def,pos);
					break;
				}
			}
		} else if (u = (def & Board->bb[WhiteKing | me])) {
            capture += piece;
			piece = 1000;
		} else return 0;
		if (capture > 500) return 1;
		if (capture - piece >= 0) return 1;
	}
}

int get_move() {
	int move, next, *p, *q, *r;
start:
    switch (Current->stage) {
		case 0: // generate captures
	    r = gen_captures(Board->bb[Current->turn ^ 1],Current->moves);
		for (q = r - 1, p = Current->moves; q > p;) {
		    move = (*q) & 0xFFFF;
		    if (False(see(move))) {
			    next = *p;
			    *p = *q;
			    *q = next;
			    p++;
		    } else q--;
	    }
		// now 0 -> (p - 1): bad captures, p -> (r - 1): good captures
		Current->start = p; // save p
		Current->current = p;
		Current->stage++;
		case 1: // browse trough the good captures
		p = Current->current;
		if (True(*p)) {
		    for (q = p + 1; (*q) != 0; q++) {
			    if ((*q) > (*p)) {
				    next = *p;
				    *p = *q;
					*q = next;
			    }
		    }
			Current->current++;
			move = (*p) & 0xFFFF;
			return move;
		} else Current->stage++;
		case 2: // first killer
		move = Current->killer[1];
		if (True(move))
			if (False(Board->square[To(move)]) && !IsEP(move) && !IsPromotion(move)) 
				if (is_legal(move)) { 
					Current->stage++; 
					return move;
				}
		Current->stage++;
		case 3: // second killer
		move = Current->killer[2];
		if (True(move))
			if (False(Board->square[To(move)]) && !IsEP(move) && !IsPromotion(move)) 
				if (is_legal(move)) {
					Current->stage++; 
					return move;
				}
		Current->stage++;
		case 4: // generate quiet moves
		gen_quiet_moves(Current->start);
		Current->current = Current->start;
		Current->stage++;
		case 5: // browse through the quiet moves
        p = Current->current;
		if (True(*p)) {
		    for (q = p + 1; (*q) != 0; q++) {
			    if ((*q) > (*p)) {
				    next = *p;
				    *p = *q;
					*q = next;
			    }
		    }
			Current->current++;
			move = (*p) & 0xFFFF;
			if (move != Current->killer[1] && move != Current->killer[2]) return move;
		} else Current->stage++;
		goto start;
		case 6: // init bad captures
		*(Current->start) = 0;
		Current->current = Current->moves;
		Current->stage++;
		case 7: // browse through the bad captures
		p = Current->current;
		if (True(*p)) {
		    for (q = p + 1; (*q) != 0; q++) {
			    if ((*q) > (*p)) {
				    next = *p;
				    *p = *q;
					*q = next;
			    }
		    }
			Current->current++;
			move = (*p) & 0xFFFF;
			return move;
		} else return 0;
		case 8: // generate evasions
		gen_evasions(Current->moves);
		Current->current = Current->moves;
		Current->stage++;
		case 9: // browse through the evasions
		p = Current->current;
		if (True(*p)) {
		    for (q = p + 1; (*q) != 0; q++) {
			    if ((*q) > (*p)) {
				    next = *p;
				    *p = *q;
					*q = next;
			    }
		    }
			Current->current++;
			move = (*p) & 0xFFFF;
			return move;
		} else return 0;
		case 10: // generate captures
		r = gen_captures(Current->mask,Current->moves);
		for (q = r - 1, p = Current->moves; q > p;) {
		    move = (*q) & 0xFFFF;
		    if (False(see(move))) {
			    next = *p;
			    *p = *q;
			    *q = next;
			    p++;
		    } else q--;
	    }
		// now 0 -> (p - 1): bad captures, p -> (r - 1): good captures
		Current->start = p; // save p
		Current->current = p;
		Current->stage++;
		case 11: // browse trough the good captures
		p = Current->current;
		if (True(*p)) {
		    for (q = p + 1; (*q) != 0; q++) {
			    if ((*q) > (*p)) {
				    next = *p;
				    *p = *q;
					*q = next;
			    }
		    }
			Current->current++;
			move = (*p) & 0xFFFF;
			return move;
		} else Current->stage++;
		case 12: // generate checks
		gen_checks(Board->bb[Current->turn ^ 1] ^ Current->mask,Current->moves);
		Current->current = Current->moves;
		Current->stage++;
		case 13: // browse through the checks
        p = Current->current;
		if (True(*p)) {
		    for (q = p + 1; (*q) != 0; q++) {
			    if ((*q) > (*p)) {
				    next = *p;
				    *p = *q;
					*q = next;
			    }
		    }
			Current->current++;
			move = (*p) & 0xFFFF;
			return move;
		} else Current->stage++;
		case 14: // init bad captures
		*(Current->start) = 0;
		Current->current = Current->moves;
		Current->stage++;
		case 15: // browse through the bad captures
		p = Current->current;
		if (True(*p)) {
		    for (q = p + 1; (*q) != 0; q++) {
			    if ((*q) > (*p)) {
				    next = *p;
				    *p = *q;
					*q = next;
			    }
		    }
			Current->current++;
			move = (*p) & 0xFFFF;
			return move;
		} else return 0;
	}
	goto start;
}

void get_attacks() {
	uint64 u, w_att, b_att;
	if (Current->eval_key == Current->key) return;
	else Current->eval_key = 0;
	w_att = Current->pawn_att[White] = ((Board->bb[WhitePawn] & (0x00FFFFFFFFFFFF00 ^ File[0])) << 7) | ((Board->bb[WhitePawn] & (0x00FFFFFFFFFFFF00 ^ File[7])) << 9);
	b_att = Current->pawn_att[Black] = ((Board->bb[BlackPawn] & (0x00FFFFFFFFFFFF00 ^ File[0])) >> 9) | ((Board->bb[BlackPawn] & (0x00FFFFFFFFFFFF00 ^ File[7])) >> 7);
	w_att |= SArea[lsb(Board->bb[WhiteKing])];
	b_att |= SArea[lsb(Board->bb[BlackKing])];
	for (u = Board->bb[WhiteKnight]; True(u); Cut(u)) w_att |= NAtt[lsb(u)];
	for (u = Board->bb[WhiteLight] | Board->bb[WhiteDark] | Board->bb[WhiteRook] | Board->bb[WhiteQueen]; True(u); Cut(u)) w_att |= Current->attacks[lsb(u)];
	for (u = Board->bb[BlackKnight]; True(u); Cut(u)) b_att |= NAtt[lsb(u)];
	for (u = Board->bb[BlackLight] | Board->bb[BlackDark] | Board->bb[BlackRook] | Board->bb[BlackQueen]; True(u); Cut(u)) b_att |= Current->attacks[lsb(u)];
	Current->all_att[White] = w_att;
	Current->all_att[Black] = b_att;
}

void eval_pawns() {
	int w_king, b_king, sq, rank, file, wk_score, bk_score, score;
	uint8 file_w, file_b, rank_w, rank_b;
	uint64 u, v, pawns, border_w, straight_w, center_w, area_w, border_b, straight_b, center_b, area_b, mask;

	w_king = lsb(Board->bb[WhiteKing]);
	b_king = lsb(Board->bb[BlackKing]);
	score = 0;
	bk_score = wk_score = -(Shelter[0][1] + Shelter[1][1] + Shelter[2][1]);
	file_w = File(w_king);
	file_b = File(b_king);
	rank_w = Rank(w_king);
	rank_b = 7 - Rank(b_king);
	straight_w = File[file_w];
	straight_b = File[file_b];
	pawns = Board->bb[WhitePawn] | Board->bb[BlackPawn];
	if (rank_w == 0) if (False(SArea[w_king] & Line[1] & (~pawns))) score -= KingBlocked;
	if (rank_b == 0) if (False(SArea[b_king] & Line[6] & (~pawns))) score += KingBlocked;
	PawnEntry->passer_w = PawnEntry->passer_b = 0;
	if (file_w > 0 && file_w < 7) {
		if (file_w < 4) {
			border_w = File[file_w - 1];
			center_w = File[file_w + 1];
		} else {
			center_w = File[file_w - 1];
			border_w = File[file_w + 1];
		}
	} else {
		border_w = 0;
		wk_score += Shelter[0][1];
		if (file_w == 7) center_w = File[6];
		else center_w = File[1];
	}
	if (file_b > 0 && file_b < 7) {
		if (file_b < 4) {
			border_b = File[file_b - 1];
			center_b = File[file_b + 1];
		} else {
			center_b = File[file_b - 1];
			border_b = File[file_b + 1];
		}
	} else {
		border_b = 0;
		bk_score += Shelter[0][1];
		if (file_b == 7) center_b = File[6];
		else center_b = File[1];
	}
	area_w = border_w | straight_w | center_w;
	area_b = border_b | straight_b | center_b;
	file_w = file_b = 0;
	for (u = Board->bb[WhitePawn]; True(u); u ^= mask) {
		sq = lsb(u);
		mask = Bit(sq);
		rank = Rank(sq);
		file = File(sq);
		file_w |= 1 << file;
		if (False(PIsolated[sq] & Board->bb[WhitePawn])) score -= PawnIsolated[rank];
		if (PWay[Black][sq] & Board->bb[WhitePawn]) {
			score -= PawnDoubled;
			if (mask & area_b) {
			    bk_score -= Storm[rank];
			    if (False(PMove[White][sq] & pawns)) bk_score -= Storm[rank];
		    }
		} else if (True(mask & area_w) && rank > rank_w) {
			if (mask & border_w) wk_score += Shelter[0][rank];
			if (mask & straight_w) wk_score += Shelter[1][rank];
			if (mask & center_w) wk_score += Shelter[2][rank];
		}
		if (mask & area_b) {
			bk_score -= Storm[rank];
		    if (False(PMove[White][sq] & pawns)) {
				bk_score -= Storm[rank];
				if (True(PAtt[Black][sq + 8] & Board->bb[WhitePawn]) && True(PAtt[White][sq + 8] & Board->bb[BlackPawn])) bk_score -= Storm[rank];
			}
		}
		if (PWay[White][sq] & Board->bb[WhitePawn]) score -= PawnDoubled;
		else if (False((PSupport[Black][sq] | PWay[White][sq]) & Board->bb[BlackPawn])) {
			score += Passer[rank];
            score += ((Abs(file - File(b_king)) + Max(rank_b + rank - 6, 0)) * PasserAtt[rank]) - ((Abs(file - File(w_king)) + Max(1 + rank - rank_w, 0)) * PasserDef[rank]);
			if (PAtt[Black][sq] & Board->bb[WhitePawn]) score += ProtectedPasser[rank];
			PawnEntry->passer_w |= 1 << file;
		}
		if (False(PAtt[Black][sq] & Board->bb[WhitePawn])) {
			score -= PawnUP[rank];
			if (False(PSupport[White][sq] & Board->bb[WhitePawn])) score -= PawnUR[rank];
			else {
			    for (v = PSupport[White][sq] & Board->bb[WhitePawn]; True(v); Cut(v))
					if (False(PSupport[White][sq] & PWay[White][lsb(v)] & pawns)) goto weak_w;
				score -= PawnUR[rank];
			}
			weak_w:
			if (True(PMove[White][sq] & pawns) || (True(PAtt[White][sq+8] & Board->bb[BlackPawn]) && False(PAtt[Black][sq+8] & Board->bb[WhitePawn]))) {
				score -= PawnWeak[rank];
				if (False(PWay[White][sq] & pawns)) score -= PawnWeakOp[rank];
			}
		}
	}
	for (u = Board->bb[BlackPawn]; True(u); u ^= mask) {
		sq = lsb(u);
		mask = Bit(sq);
		rank = 7 - Rank(sq);
		file = File(sq);
		file_b |= 1 << file;
		if (False(PIsolated[sq] & Board->bb[BlackPawn])) score += PawnIsolated[rank];
		if (PWay[White][sq] & Board->bb[BlackPawn]) {
			score += PawnDoubled;
			if (mask & area_w) {
			    wk_score -= Storm[rank];
			    if (False(PMove[Black][sq] & pawns)) wk_score -= Storm[rank];
		    }
		} else if (True(mask & area_b) && rank > rank_b) {
			if (mask & border_b) bk_score += Shelter[0][rank];
			if (mask & straight_b) bk_score += Shelter[1][rank];
		    if (mask & center_b) bk_score += Shelter[2][rank];
		}
		if (mask & area_w) {
			wk_score -= Storm[rank];
			if (False(PMove[Black][sq] & pawns)) {
				wk_score -= Storm[rank];
				if (True(PAtt[White][sq - 8] & Board->bb[BlackPawn]) && True(PAtt[Black][sq - 8] & Board->bb[WhitePawn])) wk_score -= Storm[rank];
			}
		}
		if (PWay[Black][sq] & Board->bb[BlackPawn]) score += PawnDoubled;
		else if (False((PSupport[White][sq] | PWay[Black][sq]) & Board->bb[WhitePawn])) {
			score -= Passer[rank];
            score -= ((Abs(file - File(w_king)) + Max(rank_w + rank - 6, 0)) * PasserAtt[rank]) - ((Abs(file - File(b_king)) + Max(1 + rank - rank_b, 0)) * PasserDef[rank]); 
			if (PAtt[White][sq] & Board->bb[BlackPawn]) score -= ProtectedPasser[rank];
			PawnEntry->passer_b |= 1 << file;
		}
		if (False(PAtt[White][sq] & Board->bb[BlackPawn])) {
			score += PawnUP[rank];
			if (False(PSupport[Black][sq] & Board->bb[BlackPawn])) score += PawnUR[rank];
			else {
			    for (v = PSupport[Black][sq] & Board->bb[BlackPawn]; True(v); Cut(v))
					if (False(PSupport[Black][sq] & PWay[Black][lsb(v)] & pawns)) goto weak_b;
				score += PawnUR[rank];
			}
			weak_b:
			if (True(PMove[Black][sq] & pawns) || (True(PAtt[Black][sq-8] & Board->bb[WhitePawn]) && False(PAtt[White][sq-8] & Board->bb[BlackPawn]))) {
				score += PawnWeak[rank];
				if (False(PWay[Black][sq] & pawns)) score += PawnWeakOp[rank];
			}
		}
	}
	PawnEntry->key = Low32(Current->pawn_key);
	PawnEntry->score = score;
	PawnEntry->white = file_w;
	PawnEntry->black = file_b;
	PawnEntry->w_shelter = -wk_score;
	PawnEntry->b_shelter = -bk_score;
}

template <bool HPopCnt> void evaluation() {
	int score, sq, file, rank, w_king, b_king, wk_att, bk_att, mul;
	uint32 material, king_att;
	uint64 u, v, pw_att, pb_att, w_att, b_att, att, target, up_target, threat, free, way;
#ifdef TUNABLE
	int space_w, space_b;
#endif

	if (Current->eval_key == Current->key) return;
	pw_att = Current->pawn_att[White] = ((Board->bb[WhitePawn] & (0x00FFFFFFFFFFFF00 ^ File[0])) << 7) | ((Board->bb[WhitePawn] & (0x00FFFFFFFFFFFF00 ^ File[7])) << 9);
	pb_att = Current->pawn_att[Black] = ((Board->bb[BlackPawn] & (0x00FFFFFFFFFFFF00 ^ File[0])) >> 9) | ((Board->bb[BlackPawn] & (0x00FFFFFFFFFFFF00 ^ File[7])) >> 7);
	w_king = lsb(Board->bb[WhiteKing]);
	b_king = lsb(Board->bb[BlackKing]);
	w_att = pw_att | SArea[w_king];
	b_att = pb_att | SArea[b_king];

	PawnEntry = PawnHash + (High32(Current->pawn_key) & pawn_hash_mask);
	if (PawnEntry->key != Low32(Current->pawn_key)) eval_pawns();
	score = Current->pst + PawnEntry->score;

	target = DArea[b_king];
	up_target = (~pb_att) & Board->bb[Black];
	threat = pw_att & (Board->bb[Black] ^ Board->bb[BlackPawn]);
	king_att = 0;
	free = ~(pb_att | Board->bb[White]);
	if (pw_att & SArea[b_king]) king_att += KingAttack;
	for (u = Board->bb[WhiteKnight]; True(u); u ^= way) {
		sq = lsb(u);
		att = NAtt[sq];
		way = Bit(sq);
		w_att |= att;
		if (att & target) {
			if (att & SArea[b_king]) king_att += KingNAttack;
			else king_att += KingAttack;
		}
		threat |= att & (Board->bb[BlackRook] | Board->bb[BlackQueen]);
		if (pb_att & way) continue;
		if (pw_att & way) {
			score += KnightOutpost[White][sq];
			if (Multiple(pw_att & way)) score += KnightOutpost[White][sq];
		}
		if (att & up_target) score += PieceUPAttack;
		score += popcount<HPopCnt>(free & att & Forward[0][Rank(sq)]) * KnightMobility;
	}
	for (u = Board->bb[WhiteLight] | Board->bb[WhiteDark]; True(u); u ^= way) {
		sq = lsb(u);
		att = Current->attacks[sq];
		way = Bit(sq);
		w_att |= att;
		if (att & target) {
			if (att & SArea[b_king]) king_att += KingBAttack;
			else king_att += KingAttack;
		}
		threat |= att & (Board->bb[BlackRook] | Board->bb[BlackQueen]);
		if (pb_att & way) continue;
		if (att & up_target) score += PieceUPAttack;
		score += popcount<HPopCnt>(free & att & Forward[0][Rank(sq)]) * BishopMobility; // simple "safe forward" mobility appears to be the best:)
	}
	for (u = Board->bb[WhiteRook]; True(u); u ^= way) {
		sq = lsb(u);
		att = Current->attacks[sq];
		way = Bit(sq);
		w_att |= att;
		if (att & target) {
			if (att & SArea[b_king]) {
				king_att += KingRAttack;
				score += RookKingLook;
			} else king_att += KingAttack;
		}
		threat |= att & (Board->bb[BlackQueen]);
		if (pb_att & way) continue;
		if (att & up_target) score += PieceUPAttack;
		score += popcount<HPopCnt>(free & att) * RookMobility;
		if (False(Bit(File(sq)) & PawnEntry->white)) {
			score += RookHOF;
			if (False(Bit(File(sq)) & PawnEntry->black)) {
				score += RookOF;
				if (sq < 56) {
				    rank = msb(PWay[White][sq] & Current->attacks[sq]);
				    if (rank >= 56) score += RookOFFreeLook;
					else if (Odd(Board->square[rank]) && Board->square[rank] < WhiteRook) {
					    if (PAtt[White][rank] & Board->bb[BlackPawn]) score += RookOFMinorPLook;
					    else score += RookOFMinorUPLook;
				    }
				}
			} else if (sq < 56) {
				rank = msb(PWay[White][sq] & Current->attacks[sq]);
				if (Board->square[rank] == BlackPawn) {
					if (PAtt[White][rank] & Board->bb[BlackPawn]) score += RookHOFPLook;
					else score += RookHOFUPLook;
				}
			}
		}
		if (Rank(sq) == 6)
			if (True(Board->bb[BlackKing] & Line[7]) || True(Board->bb[BlackPawn] & Line[6])) score += RookSeventhRank;
	}
	for (u = Board->bb[WhiteQueen]; True(u); u ^= way) {
		att = Current->attacks[lsb(u)];
		way = Bit(lsb(u));
		w_att |= att;
		if (att & target) {
			if (att & SArea[b_king]) {
				king_att += KingQAttack;
				for (v = att & BMask[lsb(u)] & (Board->bb[WhiteLight] | Board->bb[WhiteDark]); True(v); Cut(v)) {
					if (True(FullLine[lsb(u)][lsb(v)] & SArea[b_king]) && False(Current->attacks[lsb(v)] & target)) {
						king_att += KingAttack;
						break;
					}
				}
				for (v = att & RMask[lsb(u)] & Board->bb[WhiteRook]; True(v); Cut(v)) {
					if (True(FullLine[lsb(u)][lsb(v)] & SArea[b_king]) && False(Current->attacks[lsb(v)] & target)) {
						king_att += KingAttack;
						break;
					}
				}
			} else king_att += KingAttack;
		}
		if (pb_att & way) continue;
		if (att & up_target) score += PieceUPAttack;
		score += popcount<HPopCnt>(free & att) * QueenMobility;
	}
	wk_att = king_att & 0xFFFF;
#ifndef TUNABLE
	if (Board->bb[WhiteQueen]) score += (PawnEntry->b_shelter * ((king_att >> 16) + 80))/120;
#else
	if (Board->bb[WhiteQueen]) score += (PawnEntry->b_shelter * ((king_att >> 16) + 80))/(120 - (20 * Favor));
#endif
	score += ((king_att >> 16) * (KingAttackScale[wk_att])) >> 4;
	if (threat) {
		sq = popcount<HPopCnt>(threat);
		score += ThreatScale[sq];
	}

	target = DArea[w_king];
	up_target = (~pw_att) & Board->bb[White];
	threat = pb_att & (Board->bb[White] ^ Board->bb[WhitePawn]);
	king_att = 0;
	free = ~(pw_att | Board->bb[Black]);
	if (pb_att & SArea[w_king]) king_att += KingAttack;
	for (u = Board->bb[BlackKnight]; True(u); u ^= way) {
		sq = lsb(u);
		att = NAtt[sq];
		way = Bit(sq);
		b_att |= att;
		if (att & target) {
			if (att & SArea[w_king]) king_att += KingNAttack;
			else king_att += KingAttack;
		}
		threat |= att & (Board->bb[WhiteRook] | Board->bb[WhiteQueen]);
		if (pw_att & way) continue;
		if (pb_att & way) {
			score -= KnightOutpost[Black][sq];
			if (Multiple(pb_att & way)) score -= KnightOutpost[Black][sq];
		}
		if (att & up_target) score -= PieceUPAttack;
		score -= popcount<HPopCnt>(free & att & Forward[1][Rank(sq)]) * KnightMobility;
	}
	for (u = Board->bb[BlackLight] | Board->bb[BlackDark]; True(u); u ^= way) {
		sq = lsb(u);
		att = Current->attacks[sq];
		way = Bit(sq);
		b_att |= att;
		if (att & target) {
			if (att & SArea[w_king]) king_att += KingBAttack;
			else king_att += KingAttack;
		}
		threat |= att & (Board->bb[WhiteRook] | Board->bb[WhiteQueen]);
		if (pw_att & way) continue;
		if (att & up_target) score -= PieceUPAttack;
		score -= popcount<HPopCnt>(free & att & Forward[1][Rank(sq)]) * BishopMobility;
	}
	for (u = Board->bb[BlackRook]; True(u); u ^= way) {
		sq = lsb(u);
		att = Current->attacks[sq];
		way = Bit(sq);
		b_att |= att;
		if (att & target) {
			if (att & SArea[w_king]) {
				king_att += KingRAttack;
				score -= RookKingLook;
			} else king_att += KingAttack;
		}
		threat |= att & (Board->bb[WhiteQueen]);
		if (pw_att & way) continue;
		if (att & up_target) score -= PieceUPAttack;
		score -= popcount<HPopCnt>(free & att) * RookMobility;
		if (False(Bit(File(sq)) & PawnEntry->black)) {
			score -= RookHOF;
			if (False(Bit(File(sq)) & PawnEntry->white)) {
				score -= RookOF;
				if (sq >= 8) {
				    rank = lsb(PWay[Black][sq] & Current->attacks[sq]);
				    if (rank < 8) score -= RookOFFreeLook;
				    else if (Even(Board->square[rank]) && Board->square[rank] < WhiteRook) {
					    if (PAtt[Black][rank] & Board->bb[WhitePawn]) score -= RookOFMinorPLook;
					    else score -= RookOFMinorUPLook;
				    }
				}
			} else if (sq >= 8) {
				rank = lsb(PWay[Black][sq] & Current->attacks[sq]);
				if (Board->square[rank] == WhitePawn) {
					if (PAtt[Black][rank] & Board->bb[WhitePawn]) score -= RookHOFPLook;
					else score -= RookHOFUPLook;
				}
			}
		}
		if (Rank(sq) == 1)
			if (True(Board->bb[WhiteKing] & Line[0]) || True(Board->bb[WhitePawn] & Line[1])) score -= RookSeventhRank;
	}
	for (u = Board->bb[BlackQueen]; True(u); u ^= way) {
		att = Current->attacks[lsb(u)];
		way = Bit(lsb(u));
		b_att |= att;
		if (att & target) {
			if (att & SArea[w_king]) {
				king_att += KingQAttack;
				for (v = att & BMask[lsb(u)] & (Board->bb[BlackLight] | Board->bb[BlackDark]); True(v); Cut(v)) {
					if (True(FullLine[lsb(u)][lsb(v)] & SArea[w_king]) && False(Current->attacks[lsb(v)] & target)) {
						king_att += KingAttack;
						break;
					}
				}
				for (v = att & RMask[lsb(u)] & Board->bb[BlackRook]; True(v); Cut(v)) {
					if (True(FullLine[lsb(u)][lsb(v)] & SArea[w_king]) && False(Current->attacks[lsb(v)] & target)) {
						king_att += KingAttack;
						break;
					}
				}
			} else king_att += KingAttack;
		}
		if (pw_att & way) continue;
		if (att & up_target) score -= PieceUPAttack;
		score -= popcount<HPopCnt>(free & att) * QueenMobility;
	}
	bk_att = king_att & 0xFFFF;
#ifndef TUNABLE
	if (Board->bb[BlackQueen]) score -= Convert((PawnEntry->w_shelter * ((king_att >> 16) + 80))/120,int);
#else
	if (Board->bb[BlackQueen]) score -= Convert((PawnEntry->w_shelter * ((king_att >> 16) + 80))/(120 + (20 * Favor)),int);
#endif
	score -= Convert(((king_att >> 16) * (KingAttackScale[bk_att])) >> 4,int);
	if (threat) {
		sq = popcount<HPopCnt>(threat);
		score -= ThreatScale[sq];
	}

	Current->passer = 0;
	for (u = PawnEntry->passer_w; True(u); Cut(u)) {
		file = lsb(u);
		sq = msb(File[file] & Board->bb[WhitePawn]);
		rank = Rank(sq);
		Current->passer |= Bit(sq);
		way = PWay[White][sq];
		int connected = 0, supported = 0, hooked = 0, square;
		if (PWay[Black][sq] & (Board->bb[WhiteRook] | Board->bb[WhiteQueen])) {
			square = msb(PWay[Black][sq] & (Board->bb[WhiteRook] | Board->bb[WhiteQueen]));
			if (Current->attacks[square] & Bit(sq)) supported = 1;
		}
		if (PWay[Black][sq] & (Board->bb[BlackRook] | Board->bb[BlackQueen])) {
			square = msb(PWay[Black][sq] & (Board->bb[BlackRook] | Board->bb[BlackQueen]));
			if (Current->attacks[square] & Bit(sq)) hooked = 1;
		}
		for (v = SArea[sq] & (~File[file]) & Board->bb[WhitePawn]; True(v); Cut(v)) {
			square = lsb(v);
			if (False(Board->bb[BlackPawn] & (PWay[White][square] | PSupport[Black][square]))) connected++;
		}
		if (False(way & Board->bb[White])) score += NoSelfPasserBlock[rank];
		if (way & Board->bb[WhiteRook]) score -= RookPasserBlock[rank];
	    if (False(way & Board->bb[Black])) score += NoOppPasserBlock[rank];
	    if (False(Board->square[sq + 8])) {
			score += PasserPush[rank];
			if (False(hooked) && (connected >= 2 || False(way & (b_att | Board->bb[White] | Board->bb[Black])))) {
				score += RagingPasser[rank];
				score += FreePasser[rank];
			} else if (True(connected) || ((True(supported) || False(way & b_att & (~w_att))) & False(hooked))) score += FreePasser[rank];
		}
		if (file == 0 || file == 7) {
			if (Board->bb[Black] == (Board->bb[BlackKing] | Board->bb[BlackKnight] | Board->bb[BlackPawn]))
                score += DistantPasser[rank];
	    }
		if (Board->bb[Black] == (Board->bb[BlackPawn] | Board->bb[BlackKing])) {
			if (Rank(w_king) >= 6 && True((PIsolated[sq] | way) & Board->bb[WhiteKing])) square = file + ((Rank(w_king) - 1) << 3);
			else square = file + 56;
			hooked = Dist(b_king,square);
			if (way & Board->bb[WhiteKing]) hooked--;
			if (hooked - Current->turn > Rank(square) - rank) score += Passer[rank] + Passer[rank];
		}
	}
	for (u = PawnEntry->passer_b; True(u); Cut(u)) {
		file = lsb(u);
		sq = lsb(File[file] & Board->bb[BlackPawn]);
		rank = 7 - Rank(sq);
		Current->passer |= Bit(sq);
		way = PWay[Black][sq];
		int connected = 0, supported = 0, hooked = 0, square;
		if (PWay[White][sq] & (Board->bb[BlackRook] | Board->bb[BlackQueen])) {
			square = lsb(PWay[White][sq] & (Board->bb[BlackRook] | Board->bb[BlackQueen]));
			if (Current->attacks[square] & Bit(sq)) supported = 1;
		}
		if (PWay[White][sq] & (Board->bb[WhiteRook] | Board->bb[WhiteQueen])) {
			square = lsb(PWay[White][sq] & (Board->bb[WhiteRook] | Board->bb[WhiteQueen]));
			if (Current->attacks[square] & Bit(sq)) hooked = 1;
		}
		for (v = SArea[sq] & (~File[file]) & Board->bb[BlackPawn]; True(v); Cut(v)) {
			square = lsb(v);
			if (False(Board->bb[WhitePawn] & (PWay[Black][square] | PSupport[White][square]))) connected++;
		}
		if (False(way & Board->bb[Black])) score -= NoSelfPasserBlock[rank];
		if (way & Board->bb[BlackRook]) score += RookPasserBlock[rank];
		if (False(way & Board->bb[White])) score -= NoOppPasserBlock[rank];
	    if (False(Board->square[sq - 8])) {
			score -= PasserPush[rank];
		    if (False(hooked) && (connected >= 2 || False(way & (w_att | Board->bb[White] | Board->bb[Black])))) {
				score -= RagingPasser[rank];
				score -= FreePasser[rank];
		    } else if (True(connected) || ((True(supported) || False(way & w_att & (~b_att))) & False(hooked))) score -= FreePasser[rank];
		}
	    if (file == 0 || file == 7) {
			if (Board->bb[White] == (Board->bb[WhiteKing] | Board->bb[WhiteKnight] | Board->bb[WhitePawn]))
                score -= DistantPasser[rank];
        }
		if (Board->bb[White] == (Board->bb[WhitePawn] | Board->bb[WhiteKing])) {
			if (Rank(b_king) <= 1 && True((PIsolated[sq] | way) & Board->bb[BlackKing])) square = file + ((Rank(b_king) + 1) << 3);
			else square = file;
			hooked = Dist(w_king,square);
			if (way & Board->bb[BlackKing]) hooked--;
			if (hooked + Current->turn > 8 - Rank(square) - rank) score -= Passer[rank] + Passer[rank];
		}
	}

#ifdef TUNABLE
	if (Space) {
	    space_w = SpaceScore[popcount<HPopCnt>((Board->bb[White] ^ (Board->bb[WhiteKing] | Board->bb[WhitePawn])) & BlackArea)];
        space_b = SpaceScore[popcount<HPopCnt>((Board->bb[Black] ^ (Board->bb[BlackKing] | Board->bb[BlackPawn])) & WhiteArea)];
		if (Favor == 0) {
			score += space_w - space_b;
		} else if (Favor > 0) score += space_w;
		else score -= space_b;
	}
	if (Favor > 0) score += 5;
	else if (Favor < 0) score -= 5;
#endif

	Current->all_att[White] = w_att;
	Current->all_att[Black] = b_att;
	score += popcount<HPopCnt>(pw_att & (Board->bb[White] ^ Board->bb[WhitePawn] ^ Board->bb[WhiteKing])) * PieceOutpost;
	score -= popcount<HPopCnt>(pb_att & (Board->bb[Black] ^ Board->bb[BlackPawn] ^ Board->bb[BlackKing])) * PieceOutpost;
	score -= popcount<HPopCnt>((~w_att) & (Board->bb[White] ^ Board->bb[WhitePawn] ^ Board->bb[WhiteKing])) * PieceHanging;
	score += popcount<HPopCnt>((~b_att) & (Board->bb[Black] ^ Board->bb[BlackPawn] ^ Board->bb[BlackKing])) * PieceHanging;
	if (Current->material & FlagUnusualMaterial) {
		int dp, wmin, bmin, wr, br, wq, bq, phase;
		dp = popcount<HPopCnt>(Board->bb[WhitePawn]) - popcount<HPopCnt>(Board->bb[BlackPawn]);
		wmin = popcount<HPopCnt>(Board->bb[WhiteKnight] | Board->bb[WhiteLight] | Board->bb[WhiteDark]);
		bmin = popcount<HPopCnt>(Board->bb[BlackKnight] | Board->bb[BlackLight] | Board->bb[BlackDark]);
		wr = popcount<HPopCnt>(Board->bb[WhiteRook]);
		br = popcount<HPopCnt>(Board->bb[BlackRook]);
		wq = popcount<HPopCnt>(Board->bb[WhiteQueen]);
		bq = popcount<HPopCnt>(Board->bb[BlackQueen]);
		phase = (wmin + bmin) + (wr + br) * 2 + (wq + bq) * 4;
		Current->score = (Convert(score & 0xFFFF,sint16) * phase + Convert(score >> 16,sint16) * Max(24 - phase,0))/24;
		Current->score += dp * PawnValueOp + (wmin - bmin) * KnightValue + (wr - br) * RookValue + (wq - bq) * QueenValue;
		if (Current->turn) Current->score = -Current->score;
		Current->eval_key = Current->key;
		return;
	}
	material = Material[Current->material];
	Current->score = Convert(material & 0xFFFF,sint16) + (Opening(score) * Convert((material >> 24) & 0x1F,int) + Endgame(score) * Max(24 - Convert((material >> 24) & 0x1F,int), 0))/24;
	if (Current->score > 0) {
#ifndef TUNABLE
		if (Board->bb[BlackQueen]) 
#else
		if (True(Board->bb[BlackQueen]) && Favor <= 0)
#endif
			mul = AdvantageScale[bk_att] * ((material >> 16) & 0xF);
		else mul = ((material >> 16) & 0xF) << 4;
		if (material & MatLookForDrawW) {
			if (False(Board->bb[WhitePawn] & (~(File[0] | File[7]))) && False(Board->bb[WhiteKnight] | Board->bb[WhiteRook] | Board->bb[WhiteQueen])) {
				// 1) wrong bishop
			    if (False(Board->bb[WhitePawn] & File[7]) && False(Board->bb[WhiteLight]) && True(Board->bb[BlackKing] & 0x0303000000000000)) mul = Min(mul, 1 << 4);
			    if (False(Board->bb[WhitePawn] & File[0]) && False(Board->bb[WhiteDark]) && True(Board->bb[BlackKing] & 0xC0C0000000000000)) mul = Min(mul, 1 << 4);
				// 2) bishop & blocked pawn
				if (True(Board->bb[WhitePawn] & Line[5] & (Board->bb[BlackPawn] >> 8))) {
			        if (False(Board->bb[WhitePawn] & File[7]) && True(Board->bb[BlackKing] & 0x0707000000000000)) mul = Min(mul, 1 << 4);
			        if (False(Board->bb[WhitePawn] & File[0]) && True(Board->bb[BlackKing] & 0xE0E0000000000000)) mul = Min(mul, 1 << 4);
			    }
			}
		}
	} else {
#ifndef TUNABLE
		if (Board->bb[WhiteQueen])
#else
		if (True(Board->bb[WhiteQueen]) && Favor >= 0)
#endif
			mul = AdvantageScale[wk_att] * ((material >> 20) & 0xF);
		else mul = ((material >> 20) & 0xF) << 4;
		if (material & MatLookForDrawB) {
	        if (False(Board->bb[BlackPawn] & (~(File[0] | File[7]))) && False(Board->bb[BlackKnight] | Board->bb[BlackRook] | Board->bb[BlackQueen])) {
				// 1) wrong bishop
			    if (False(Board->bb[BlackPawn] & File[7]) && False(Board->bb[BlackDark]) && True(Board->bb[WhiteKing] & 0x0000000000000303)) mul = Min(mul, 1 << 4);
			    if (False(Board->bb[BlackPawn] & File[0]) && False(Board->bb[BlackLight]) && True(Board->bb[WhiteKing] & 0x000000000000C0C0)) mul = Min(mul, 1 << 4);
				// 2) bishop & blocked pawn
				if (True(Board->bb[BlackPawn] & Line[2] & (Board->bb[WhitePawn] << 8))) {
			        if (False(Board->bb[BlackPawn] & File[7]) && True(Board->bb[WhiteKing] & 0x0000000000000707)) mul = Min(mul, 1 << 4);
			        if (False(Board->bb[BlackPawn] & File[0]) && True(Board->bb[WhiteKing] & 0x000000000000E0E0)) mul = Min(mul, 1 << 4);
			    }
		    }
		}
	}
	Current->score = (Current->score * mul) / 240;
	if (Current->turn) Current->score = -Current->score;
	Current->eval_key = Current->key;
}

inline void evaluate() {
	HardwarePopCnt ? evaluation<1>() : evaluation<0>();
}

int is_legal(int move) {
	int from, to, piece, capture;
	uint64 u, occ;

    from = From(move);
	to = To(move);
	piece = Board->square[from];
	capture = Board->square[to];
	if (piece == 0) return 0;
	if ((piece & 1) != Current->turn) return 0;
	occ = Board->bb[White] | Board->bb[Black];
	u = Bit(to);
	if (piece >= WhiteLight && piece < WhiteKing) {
	    if ((QMask[from] & u) == 0) return 0;
		if (Between[from][to] & occ) return 0;
	}
	if (True(capture) && (capture & 1) == (piece & 1)) return 0;
	if (IsEP(move)) {
		if (piece >= WhiteKnight) return 0;
		if (Current->ep_square != to) return 0;
		return 1;
	}
	if (IsCastling(move) && Board->square[from] < WhiteKing) return 0;
	if (IsPromotion(move) && Board->square[from] >= WhiteKnight) return 0;
	if (Current->turn == White) {
		if (piece == WhitePawn) {
			if (u & PMove[White][from]) {
                if (capture) return 0;
				if ((u & 0xFF00000000000000) && !IsPromotion(move)) return 0;
				return 1;
			} else if (to == from + 16) {
                if (capture) return 0;
				if (Board->square[to - 8]) return 0;
				if (False(u & 0x00000000FF000000)) return 0;
				return 1;
			} else if (u & PAtt[White][from]) {
                if (capture == 0) return 0;
				if ((u & 0xFF00000000000000) && !IsPromotion(move)) return 0;
				return 1;
			} else return 0;
		} else if (piece == WhiteKing) {
			if (IsCastling(move)) {
				if (u & 0x40) {
                    if (((Current->flags) & CanCastle_OO) == 0) return 0;
					if (occ & 0x60) return 0;
					if (Current->all_att[Black] & 0x20) return 0;
				} else {
					if (((Current->flags) & CanCastle_OOO) == 0) return 0;
					if (occ & 0xE) return 0;
					if (Current->all_att[Black] & 0x08) return 0;
				}
				return 1;
			}
            if (False(SArea[from] & u)) return 0;
			if (Current->all_att[Black] & u) return 0;
			return 1;
		}
	} else {
        if (piece == BlackPawn) {
			if (u & PMove[Black][from]) {
                if (capture) return 0;
				if ((u & 0x00000000000000FF) && !IsPromotion(move)) return 0;
				return 1;
			} else if (to == from - 16) {
                if (capture) return 0;
				if (Board->square[to + 8]) return 0;
				if (False(u & 0x000000FF00000000)) return 0;
				return 1;
			} else if (u & PAtt[Black][from]) {
                if (capture == 0) return 0;
				if ((u & 0x00000000000000FF) && !IsPromotion(move)) return 0;
				return 1;
			} else return 0;
		} else if (piece == BlackKing) {
			if (IsCastling(move)) {
				if (u & 0x4000000000000000) {
                    if (((Current->flags) & CanCastle_oo) == 0) return 0;
					if (occ & 0x6000000000000000) return 0;
					if (Current->all_att[White] & 0x2000000000000000) return 0;
				} else {
					if (((Current->flags) & CanCastle_ooo) == 0) return 0;
					if (occ & 0x0E00000000000000) return 0;
					if (Current->all_att[White] & 0x0800000000000000) return 0;
				}
				return 1;
			}
            if (False(SArea[from] & u)) return 0;
			if (Current->all_att[White] & u) return 0;
			return 1;
		}
	}
	piece = (piece >> 1) - 2;
	if (piece == 0) {
        if (u & NAtt[from]) return 1;
		else return 0;
	} else {
		if (Current->attacks[from] & u) return 1;
		else return 0;
	}
	return 0;
}

void hash_high(int value, int depth) {
	int i;
	uint64 score, best_score;
	GEntry *best;

	best_score = 0xFFFFFFFFFFFFFFFF;
	for (i = 0, best = Entry = Hash + (High32(Current->key) & hash_mask); i < 4; i++, Entry++) {
		if (Entry->key == Low32(Current->key)) {
			UpdateDate(Entry->date, date);
			if (Entry->low_depth <= depth && Entry->low > value) Entry->low = value;
			if (Entry->high_depth < depth) {
				if (Entry->high >= value || False(Entry->high_depth)) {
				    Entry->high_depth = depth;
				    Entry->high = value;
				    goto sweep;
				}
			} else if (Entry->high_depth == depth) {
				if (Entry->high > value) Entry->high = value;
				goto sweep;
			} else if (Entry->high <= value) return;
			score = (Convert(Entry->date,uint64) << 32) | Convert(Entry->high_depth,uint64);
		} else score = (Convert(Entry->date,uint64) << 32) | Convert(Max(Entry->high_depth, Entry->low_depth),uint64);
		if (score < best_score) {
			best_score = score;
			best = Entry;
		}
	}
	best->date = date;
	if (best->key == Low32(Current->key)) {
		best->high = value;
		best->high_depth = depth;
	} else {
		best->key = Low32(Current->key);
		best->high = value;
		best->high_depth = depth;
		best->low = 0;
		best->low_depth = 0;
		best->move = 0;
	}
	return;
sweep:
	for (i = i + 1, Entry = Entry + 1; i < 4; i++, Entry++) {
		if (Entry->key == Low32(Current->key)) {
			UpdateDate(Entry->date, date);
		    if (Entry->low_depth <= depth && Entry->low > value) Entry->low = value;
		    if (Entry->high_depth <= depth && Entry->high >= value) {
				Entry->high_depth = 0;
				Entry->high = value;
			}
		}
	}
	return;
}

void hash_low(int move, int value, int depth) {
	int i;
	uint64 score, best_score;
	GEntry *best;

	best_score = 0xFFFFFFFFFFFFFFFF;
	move &= 0xFFFF;
	for (i = 0, best = Entry = Hash + (High32(Current->key) & hash_mask); i < 4; i++, Entry++) {
		if (Entry->key == Low32(Current->key)) {
			UpdateDate(Entry->date, date);
			if (Entry->high_depth <= depth && Entry->high < value) Entry->high = value;
			if (Entry->low_depth < depth) {
				if (Entry->low <= value) {
				    Entry->low_depth = depth;
				    Entry->low = value;
				    if (move) Entry->move = move;
				    goto sweep;
				}
			} else if (Entry->low_depth == depth) {
				if (Entry->low < value) {
					Entry->low = value;
					if (move) Entry->move = move; 
				}
				goto sweep;
			} else if (Entry->low >= value) return;
		    score = (Convert(Entry->date,uint64) << 32) | Convert(Entry->low_depth,uint64);
		} else score = (Convert(Entry->date,uint64) << 32) | Convert(Max(Entry->high_depth, Entry->low_depth),uint64);
		if (score < best_score) {
			best_score = score;
			best = Entry;
		}
	}
	best->date = date;
	if (best->key == Low32(Current->key)) {
		best->low = value;
		best->low_depth = depth;
		if (move) best->move = move;
	} else {
		best->key = Low32(Current->key);
		best->high = 0;
		best->high_depth = 0;
		best->low = value;
		best->low_depth = depth;
		best->move = move;
	}
	return;
sweep:
	for (i = i + 1, Entry = Entry + 1; i < 4; i++, Entry++) {
		if (Entry->key == Low32(Current->key)) {
			UpdateDate(Entry->date, date);
		    if (Entry->high_depth <= depth && Entry->high < value) Entry->high = value;
		    if (Entry->low_depth <= depth && Entry->low <= value) {
				Entry->low = value;
				Entry->low_depth = 0;
			}
		}
	}
	return;
}

void hash_high_pv(int value, int depth) {
	int i;
	uint64 score, best_score;
	GEntry *best;

	best_score = 0xFFFFFFFFFFFFFFFF;
	for (i = 0, best = Entry = Hash + (High32(Current->key) & hash_mask); i < 4; i++, Entry++) {
		if (Entry->key == Low32(Current->key)) {
			UpdateDate(Entry->date, date);
			if (Entry->low_depth <= depth && Entry->low > value) Entry->low = value;
			if (Entry->high_depth < depth) {
				if (Entry->high >= value || False(Entry->high_depth)) {
				    Entry->high_depth = depth;
				    Entry->high = value;
				    goto sweep;
				}
			} else if (Entry->high_depth == depth) {
				if (Entry->high > value) Entry->high = value;
				goto sweep;
			} else if (Entry->high <= value) return;
			score = (Convert(Entry->date,uint64) << 32) | Convert(Entry->high_depth,uint64);
		} else score = (Convert(Entry->date,uint64) << 32) | Convert(Max(Entry->high_depth, Entry->low_depth),uint64);
		if (score < best_score) {
			best_score = score;
			best = Entry;
		}
	}
	best->date = date;
	if (best->key == Low32(Current->key)) {
		best->high = value;
		best->high_depth = depth;
	} else {
		best->key = Low32(Current->key);
		best->high = value;
		best->high_depth = depth;
		best->low = 0;
		best->low_depth = 0;
		best->move = 0;
	}
	return;
sweep:
	for (i = i + 1, Entry = Entry + 1; i < 4; i++, Entry++) {
		if (Entry->key == Low32(Current->key)) {
			UpdateDate(Entry->date, date);
		    if (Entry->low_depth <= depth && Entry->low > value) Entry->low = value;
		    if (Entry->high_depth <= depth && Entry->high >= value) {
				Entry->high_depth = 0;
				Entry->high = value;
			}
		}
	}
	return;
}

void hash_low_pv(int move, int value, int depth) {
	int i;
	uint64 score, best_score;
	GEntry *best;

	best_score = 0xFFFFFFFFFFFFFFFF;
	move &= 0xFFFF;
	for (i = 0, best = Entry = Hash + (High32(Current->key) & hash_mask); i < 4; i++, Entry++) {
		if (Entry->key == Low32(Current->key)) {
			UpdateDate(Entry->date, date);
			if (Entry->high_depth <= depth && Entry->high < value) Entry->high = value;
			if (Entry->low_depth < depth) {
				if (Entry->low <= value) {
				    Entry->low_depth = depth;
				    Entry->low = value;
				    if (move) Entry->move = move;
				    goto sweep;
				}
			} else if (Entry->low_depth == depth) {
				if (Entry->low < value) {
					Entry->low = value;
					if (move) Entry->move = move; 
				}
				goto sweep;
			} else if (Entry->low >= value) return;
		    score = (Convert(Entry->date,uint64) << 32) | Convert(Entry->low_depth,uint64);
		} else score = (Convert(Entry->date,uint64) << 32) | Convert(Max(Entry->high_depth, Entry->low_depth),uint64);
		if (score < best_score) {
			best_score = score;
			best = Entry;
		}
	}
	best->date = date;
	if (best->key == Low32(Current->key)) {
		best->low = value;
		best->low_depth = depth;
		if (move) best->move = move;
	} else {
		best->key = Low32(Current->key);
		best->high = 0;
		best->high_depth = 0;
		best->low = value;
		best->low_depth = depth;
		best->move = move;
	}
	return;
sweep:
	for (i = i + 1, Entry = Entry + 1; i < 4; i++, Entry++) {
		if (Entry->key == Low32(Current->key)) {
			UpdateDate(Entry->date, date);
		    if (Entry->high_depth <= depth && Entry->high < value) Entry->high = value;
		    if (Entry->low_depth <= depth && Entry->low <= value) {
				Entry->low_depth = 0;
				Entry->low = value;
			}
		}
	}
	return;
}

#ifdef TUNABLE
void hash_high_forced(int value) {
	int i;
	uint64 score, best_score;
	GEntry *best;

	best_score = 0xFFFFFFFFFFFFFFFF;
	for (i = 0, best = Entry = Hash + (High32(Current->key) & hash_mask); i < 4; i++, Entry++) {
		if (Entry->key == Low32(Current->key)) {
			Entry->date = date + 1;
			if (Entry->low > value) Entry->low = value;
			if (Entry->high >= value) {
				Entry->high_depth = 127;
				Entry->high = value;
				Entry->date = 0x10000000;
				goto sweep;
			}
			score = (Convert(Entry->date,uint64) << 32) | Convert(Entry->high_depth,uint64);
		} else score = (Convert(Entry->date,uint64) << 32) | Convert(Max(Entry->high_depth, Entry->low_depth),uint64);
		if (score < best_score) {
			best_score = score;
			best = Entry;
		}
	}
	best->date = 0x10000000;
	if (best->key == Low32(Current->key)) {
		best->high = value;
		best->high_depth = 127;
	} else {
		best->key = Low32(Current->key);
		best->high = value;
		best->high_depth = 127;
		best->low = 0;
		best->low_depth = 0;
		best->move = 0;
	}
	return;
sweep:
	for (i = i + 1, Entry = Entry + 1; i < 4; i++, Entry++) {
		if (Entry->key == Low32(Current->key)) {
			Entry->date = date + 1;
		    if (Entry->low > value) Entry->low = value;
		    if (Entry->high >= value) {
				Entry->high_depth = 0;
				Entry->date = 0;
			}
		}
	}
	return;
}

void hash_low_forced(int value) {
	int i;
	uint64 score, best_score;
	GEntry *best;

	best_score = 0xFFFFFFFFFFFFFFFF;
	for (i = 0, best = Entry = Hash + (High32(Current->key) & hash_mask); i < 4; i++, Entry++) {
		if (Entry->key == Low32(Current->key)) {
			Entry->date = date + 1;
			if (Entry->high < value) Entry->high = value;
			if (Entry->low <= value) {
				Entry->low_depth = 127;
				Entry->low = value;
				Entry->date = 0x10000000;
				goto sweep;
			}
		    score = (Convert(Entry->date,uint64) << 32) | Convert(Entry->low_depth,uint64);
		} else score = (Convert(Entry->date,uint64) << 32) | Convert(Max(Entry->high_depth, Entry->low_depth),uint64);
		if (score < best_score) {
			best_score = score;
			best = Entry;
		}
	}
	best->date = 0x10000000;
	if (best->key == Low32(Current->key)) {
		best->low = value;
		best->low_depth = 127;
	} else {
		best->key = Low32(Current->key);
		best->high = 0;
		best->high_depth = 0;
		best->low = value;
		best->low_depth = 127;
		best->move = 0;
	}
	return;
sweep:
	for (i = i + 1, Entry = Entry + 1; i < 4; i++, Entry++) {
		if (Entry->key == Low32(Current->key)) {
			Entry->date = date + 1;
		    if (Entry->high < value) Entry->high = value;
		    if (Entry->low <= value) {
				Entry->low_depth = 0;
				Entry->date = 0;
			}
		}
	}
	return;
}
#endif

void hash_exact(int move, int value, int depth) {
	int i;
	uint64 score, best_score;
	GPVEntry *best;

	best_score = 0xFFFFFFFFFFFFFFFF;
	for (i = 0, best = PVEntry = PVHash + (High32(Current->key) & pv_hash_mask); i < pv_cluster_size; i++, PVEntry++) {
		if (PVEntry->key == Low32(Current->key)) {
			PVEntry->date = date;
			if (PVEntry->depth <= depth) {
				PVEntry->value = value;
				PVEntry->depth = depth;
				PVEntry->move = move;
			}
			return;
		}
		score = (Convert(PVEntry->date,uint64) << 32) | Convert(PVEntry->depth,uint64);
		if (score < best_score) {
			best_score = score;
			best = PVEntry;
		}
	}
	best->key = Low32(Current->key);
	best->date = date;
	best->value = value;
	best->depth = depth;
	best->move = move;
}

inline int extension(int move, int pv) {
	int ext = 0;
	int from = From(move);
	int to = To(move);
	if (False(pv)) {
		if (Current->passer & (Bit(to) | Bit(from))) if (ext < 1) ext = 1;
	} else {
		if (Board->square[to] >= WhiteKnight || True(move & 0xE000)) if (see(move)) ext = 1;
	    if (Current->passer & (Bit(to) | Bit(from))) {
			if (Current->passer & Board->bb[Current->turn] & Bit(from)) {
			    if (Current->turn) {
				    if (from < 24) ext = 2;
			    } else {
				    if (from >= 40) ext = 2;
			    }
		    }
			if (ext < 1) ext = 1;
		}
	}
	return ext;
}

inline int is_check(int move) { // this function is not precise
	uint64 king, occ, u;
	int me, from, to, piece, king_sq;

	me = Current->turn;
	from = From(move);
	to = To(move);
	piece = Board->square[from] >> 1;
	king = Board->bb[BlackKing ^ me];
	king_sq = lsb(king);
	occ = Board->bb[White] | Board->bb[Black];
	u = Between[king_sq][to];
	// direct checks
	if (piece == 1) {
		if (PAtt[me][to] & king) return 1;
	} else if (piece == 2) {
		if (NAtt[to] & king) return 1;
	} else if (piece <= 4) {
		if (BMask[to] & king) if (False(u & occ)) return 1;
	} else if (piece <= 5) {
		if (RMask[to] & king) if (False(u & occ)) return 1;
	} else if (piece <= 6) {
		if (QMask[to] & king) if (False(u & occ)) return 1;
	}
	u = Between[king_sq][from];
	if (False(u)) return 0;
	if (u & occ) return 0;
	// hidden checks
	for (u = FullLine[king_sq][from] & (Board->bb[WhiteLight | me] | Board->bb[WhiteDark | me] | Board->bb[WhiteRook | me] | Board->bb[WhiteQueen | me]); True(u); Cut(u)) {
		if (Current->attacks[lsb(u)] & Bit(from)) return 1;
	}
	return 0;
}

inline int is_defence(int move, uint64 target) {
	int from, to, piece, sq;

	if (False(target)) return 0;
	from = From(move);
	if (Bit(from) & target) return 1;
	to = To(move);
	piece = Board->square[from];
	if (piece >= WhiteKing) return 0;
	if (piece < WhiteKnight) {
		if (PAtt[Current->turn][to] & target) return 1;
	} else if (piece < WhiteLight) {
		if (NAtt[to] & target) return 1;
	} else if (piece < WhiteRook) {
		if (Single(target)) {
			sq = lsb(target);
			if (True(Between[sq][to] & BMask[sq]) && False(Current->attacks[from] & target)) if (False(Between[sq][to] & (Board->bb[White] | Board->bb[Black]))) return 1;
		} else if (BishopAttacks(to, Board->bb[White] | Board->bb[Black]) & (target & (~Current->attacks[from]))) return 1;
	} else if (piece < WhiteQueen) {
		if (Single(target)) {
			sq = lsb(target);
			if (True(Between[sq][to] & RMask[sq]) && False(Current->attacks[from] & target)) if (False(Between[sq][to] & (Board->bb[White] | Board->bb[Black]))) return 1;
		} else if (RookAttacks(to, Board->bb[White] | Board->bb[Black]) & (target & (~Current->attacks[from]))) return 1;
	} else {
		if (Single(target)) {
			sq = lsb(target);
			if (True(Between[sq][to]) && False(Current->attacks[from] & target)) if (False(Between[sq][to] & (Board->bb[White] | Board->bb[Black]))) return 1;
		} else if (QueenAttacks(to, Board->bb[White] | Board->bb[Black]) & (target & (~Current->attacks[from]))) return 1;
	}
	return 0;
}

int q_search(int beta, int depth, int flags) {
	int i, move, me, hash_move, hash_depth = -256, value, *p, *q, new_depth, cpt, flag, curr_value;
	uint64 target;

	if (flags & FlagHaltCheck) {
	    if ((Current - Data) - MateValue >= beta) return beta;
	    if (MateValue - (Current - Data) < beta) return beta - 1;
	    halt_check;
	}
	Current->killer[0] = hash_move = flags & 0xFFFF;
	if (flags & FlagHashCheck) {
	    hash_depth = 0;
	    for (i = 0, Entry = Hash + (High32(Current->key) & hash_mask); i < 4; i++, Entry++) {
		    if (Low32(Current->key) == Entry->key) {
			    if (Entry->high_depth >= depth && Entry->high < beta) return Entry->high;
			    if (Entry->low_depth >= depth && Entry->low >= beta) return Entry->low;
			    if (Entry->move && Entry->low_depth > hash_depth) {
				    Current->killer[0] = hash_move = Entry->move;
				    hash_depth = Entry->low_depth;
			    }
		    }
	    }
	}
	if (flags & FlagCallEvaluation) evaluate();
	me = Current->turn;
	if (Board->bb[BlackKing ^ me] & Current->all_att[me]) return MateValue + 1;
	if (Board->bb[WhiteKing | me] & Current->all_att[me ^ 1]) return q_evasion(beta,depth,hash_move | (Current->score < beta - 5 ? FlagOnlyCaptures : 0));
	if (Current->score >= beta - 5) {
		hash_low(0, Current->score + 5, 16);
		return Current->score + 5;
	}
	curr_value = Current->score + 5;
	UpdateDeltaM;
	new_depth = depth - 1;
	if (new_depth < 1) new_depth = 1;
	cpt = 0;
	flag = 0;
	target = Board->bb[Black ^ me];
	if (Current->score + 150 < beta) {
		if (Current->all_att[me] & Board->bb[BlackPawn ^ me]) {
		    target ^= Board->bb[BlackPawn ^ me];
			curr_value = Current->score + 150;
		}
		if (Current->score + 450 < beta) {
			if (Current->all_att[me] & (Board->bb[BlackKnight ^ me] | Board->bb[BlackLight ^ me] | Board->bb[BlackDark ^ me])) {
				target ^= Board->bb[BlackKnight ^ me] | Board->bb[BlackLight ^ me] | Board->bb[BlackDark ^ me];
			    curr_value = Current->score + 450;
			}
			if (Current->score + 650 < beta) {
				if (Current->all_att[me] & Board->bb[BlackRook ^ me]) {
					target ^= Board->bb[BlackRook ^ me];
				    curr_value = Current->score + 650;
				}
			}
		}
	}
	if (True(hash_move) && False(Board->square[To(hash_move)]) && False(hash_move & 0xD000)) {
		if (is_check(hash_move)) flag = 1;
		else {
			if (Current->score + 30 < beta || depth <= 14) hash_move = 0;
			goto checks;
		}
	}
	if (hash_move) {
		if (is_legal(hash_move)) {
			if (Board->square[To(hash_move)]) cpt++; 
		    do_move(hash_move);
		    value = -q_search(1 - beta, new_depth, FlagNeatSearch);
		    undo_move(hash_move);
			if (value > curr_value) {
				curr_value = value; 
		        if (value >= beta) {
			        hash_low(hash_move,value,Max(16, depth));
			        return value;
		        }
			}
		} else Current->killer[0] = 0;
	}
	if (flag) goto checks;
	if (True(hash_move)) {
		if (True(Board->square[To(hash_move)])) {
			if (False(Bit(To(hash_move)) & target)) goto checks;
			flag = 1;
		}
	}
	gen_captures(target,Current->moves);
	for (p = Current->moves; *p != 0; p++) {
		move = *p;
		for (q = p + 1; *q != 0; q++) {
			if (*q > *p) {
				*p = *q;
				*q = move;
				move = *p;
			}
		}
        move &= 0xFFFF;
		if (move == hash_move) {
			flag = 0;
			continue;
		} else if (flag) continue;
		if (False(see(move))) continue;
		else cpt++;
		do_move(move);
		value = -q_search(1 - beta, new_depth, FlagNeatSearch);
		undo_move(move);
		if (value > curr_value) {
			curr_value = value;
		    if (value >= beta) {
			    hash_low(move,value,Max(16, depth));
			    Current->killer[0] = move;
			    return value;
		    }
		}
	}
checks:
	if (depth >= 14) {
		Current->gen_flags |= FlagSkipBadChecks;
		gen_checks(Board->bb[Black ^ me] ^ target,Current->moves);
		for (p = Current->moves; *p != 0; p++) {
		    move = *p;
		    for (q = p + 1; *q != 0; q++) {
			    if (*q > *p) {
				    *p = *q;
				    *q = move;
				    move = *p;
			    }
		    }
            move &= 0xFFFF;
			if (move == hash_move) continue;
			if (False(Bit(From(move)) & Current->xray)) if (False(see(move))) continue;
			do_move(move);
			value = -q_evasion(1 - beta, new_depth, FlagNeatSearch);
			undo_move(move);
			if (value > curr_value) {
			    curr_value = value;
		        if (value >= beta) {
			        hash_low(move,value,Max(16, depth));
			        Current->killer[0] = move;
			        return value;
		        }
		    }
		}
	}

	if (depth >= 15 && False(cpt) && Current->score > beta - 30) {
		Current->margin = beta - Current->score;
		gen_delta_moves(Current->moves);
		for (p = Current->moves; *p != 0; p++) {
		    move = *p;
		    for (q = p + 1; *q != 0; q++) {
			    if (*q > *p) {
				    *p = *q;
				    *q = move;
				    move = *p;
			    }
		    }
            move &= 0xFFFF;
			if (move == hash_move) continue;
			if (False(see(move))) continue;
			cpt++;
			do_move(move);
			value = -q_search(1 - beta, new_depth, FlagNeatSearch);
			undo_move(move);
			if (value > curr_value) {
			    curr_value = value;
		        if (value >= beta) {
			        hash_low(move,value,Max(16, depth));
			        Current->killer[0] = move;
			        return value;
		        }
		    }
			if (cpt >= 3) break;
		}
	}

	hash_high(curr_value,Max(16, depth));
	return curr_value;
}

int q_evasion(int beta, int depth, int flags) {
	int i, value, move, *p, *q, new_depth, hash_move, hash_depth, curr_value;
	
	curr_value = (Current - Data) - MateValue;
	if (flags & FlagHaltCheck) {
	    if (curr_value >= beta) return beta;
	    if (-curr_value < beta) return beta - 1;
	    halt_check;
	}
	if (flags & FlagOnlyCaptures) curr_value = beta - 1;
	Current->killer[0] = hash_move = flags & 0xFFFF;
	if (flags & FlagHashCheck) {
	    hash_depth = 0;
	    for (i = 0, Entry = Hash + (High32(Current->key) & hash_mask); i < 4; i++, Entry++) {
		    if (Low32(Current->key) == Entry->key) {
			    if (Entry->high_depth >= depth && Entry->high < beta) return Entry->high;
			    if (Entry->low_depth >= depth && Entry->low >= beta) return Entry->low;
			    if (Entry->move && Entry->low_depth > hash_depth) {
				    Current->killer[0] = hash_move = Entry->move;
				    hash_depth = Entry->low_depth;
			    }
		    }
	    }
	}
	if (flags & FlagCallEvaluation) get_attacks();
	if (Board->bb[BlackKing ^ Current->turn] & Current->all_att[Current->turn]) return MateValue + 1;
	new_depth = depth - 1;
	if (new_depth < 1) new_depth = 1;
	if (hash_move) {
		if (is_legal(hash_move)) {
			do_move(hash_move);
			value = -q_search(1 - beta, new_depth, FlagNeatSearch | FlagDisableDelta);
			undo_move(hash_move);
			if (value > curr_value) {
				curr_value = value;
			    if (value >= beta) {
				    hash_low(hash_move,value,Max(16, depth));
			        return value;
			    }
			}
		} else Current->killer[0] = 0;
	}
	gen_evasions(Current->moves);
	for (p = Current->moves; *p != 0; p++) {
		move = *p;
		for (q = p + 1; *q != 0; q++) {
			if (*q > *p) {
				*p = *q;
				*q = move;
				move = *p;
			}
		}
        move &= 0xFFFF;
		if (move == hash_move) continue;
		if (True(flags & FlagOnlyCaptures)) {
			if (False(Board->square[To(move)]) && False(move & 0xE000)) break;
			if (False(see(move))) break;
		}
		do_move(move);
		value = -q_search(1 - beta, new_depth, FlagNeatSearch | FlagDisableDelta);
		undo_move(move);
		if (value > curr_value) {
			curr_value = value;
		    if (value >= beta) {
			    hash_low(move,value,Max(16, depth));
			    Current->killer[0] = move;
			    return value;
		    }
		}
	}
	
	hash_high(curr_value, Max(16, depth));
	return curr_value;
}

int search(int beta, int depth, int flags) {
	int i, cnt, move, check, ext, me, hash_move, static_value, prune, singular, hash_depth, hash_value = -MateValue, value, new_depth, *p, curr_value;
	uint64 target;
#ifndef EPD_TESTING
	if (nodes > check_node + 16384) {
		check_node = nodes;
		check_time(1);
	}
#ifdef KNS_TESTING
	if (True(NodeLimit) && nodes >= NodeLimit * KN_FACTOR) longjmp(Jump,1);
#endif
#else
	if (nodes >= MaxNodes) longjmp(Jump,1);
#endif
	if (flags & FlagHaltCheck) {
	    if ((Current - Data) - MateValue >= beta) return beta;
	    if (MateValue - (Current - Data) < beta) return beta - 1;
	    halt_check;
	}
	Current->killer[0] = hash_move = flags & 0xFFFF;
	hash_depth = 0;
	if (True(flags & FlagHashCheck) || depth > 24) {
	    for (i = 0, Entry = Hash + (High32(Current->key) & hash_mask); i < 4; i++, Entry++) {
		    if (Low32(Current->key) == Entry->key) {
				UpdateDate(Entry->date, date);
		        if (Entry->high_depth >= depth && Entry->high < beta) return Entry->high;
			    if (Entry->low_depth >= depth && Entry->low >= beta) {
					if (Entry->move) Current->killer[0] = Entry->move;
					return Entry->low;
				}
			    if (Entry->move && Entry->low_depth > hash_depth) {
				    Current->killer[0] = hash_move = Entry->move;
				    hash_depth = Entry->low_depth;
			        hash_value = Entry->low;
			    }
		    }
	    }
	}
	if (flags & FlagCallEvaluation) evaluate();
	if (beta > -EvalValue && beta < EvalValue) prune = 1;
	else prune = 0;
	me = Current->turn;
	if (Board->bb[BlackKing ^ me] & Current->all_att[me]) return MateValue + 1;
	if (depth > 24) {
		if (False(hash_move) || hash_depth < depth - 12 || hash_value < beta - ExclusionSingle) {
			new_depth = depth - 8;
			value = search(beta, new_depth, hash_move | FlagHashCheck | FlagDisableDelta);
			if (value >= beta) {
				hash_move = Current->killer[0];
				hash_value = value;
				hash_depth = new_depth;
			} else if (True(prune)) {
				value = search(beta - ExclusionSingle, new_depth, hash_move | FlagHashCheck | FlagDisableDelta);
				if (value >= beta - ExclusionSingle) {
					hash_move = Current->killer[0];
					hash_value = value;
					hash_depth = new_depth;
				}
			}
		}
	}
	if (Board->bb[WhiteKing | me] & Current->all_att[me ^ 1]) return search_evasion(beta,depth,hash_move);
	static_value = Current->score;
	UpdateDeltaM;

	curr_value = Min(beta - 1, static_value + 5);
	if (True(prune)) {
		if (depth <= 18) {
			if (static_value + 12 + (Max(depth - 16, 0) << 3) < beta) return Max(static_value + 12 + (Max(depth - 16, 0) << 3), q_search(beta, 16, FlagHashCheck | hash_move));
			if (static_value - (50 + (Max(depth - 16, 0) << 4)) >= beta) {
				hash_low(0, static_value - (50 + (Max(depth - 16, 0) << 4)), depth);
				return static_value - (50 + (Max(depth - 16, 0) << 4));
			}
		} else if (static_value - (100 + (Max(depth - 16, 0) << 5)) >= beta && depth <= 22) {
			hash_low(0, static_value - (100 + (Max(depth - 16, 0) << 5)), depth);
			return static_value - (100 + (Max(depth - 16, 0) << 5));
		}
    }  

	target = 0;
	if ((static_value > beta + 5 || ((static_value >= beta || hash_value >= beta) && depth > 24)) 
		&& True(prune) && False(flags & FlagNullSearch) && True((Board->bb[me] ^ Board->bb[WhiteKing | me]) & (~Board->bb[WhitePawn | me]))) {
		do_null();
		new_depth = depth - 8;
		if (new_depth <= 16) value = -q_search(1 - beta, new_depth, FlagHashCheck | FlagCallEvaluation | FlagNullSearch | FlagDisableDelta);
		else value = -search(1 - beta, new_depth, FlagHashCheck | FlagCallEvaluation | FlagNullSearch | FlagDisableDelta);
		undo_null();
		if (value >= beta) {
			hash_low(0,value,Max(24, depth));
			return value;
		} else if (True((Current + 1)->killer[0]) && True(Board->square[To((Current + 1)->killer[0])])) Add(target, To((Current + 1)->killer[0]));
	}

	singular = 0;
	ext = extension(hash_move,0);
	if (ext > 1) goto skip_exclusion;
	if (depth >= 32 && True(hash_move) && hash_depth >= depth - 12 && is_legal(hash_move) && hash_value > -EvalValue && True(prune)) {
		new_depth = depth - Min(14, (depth - 8) >> 1);
		if (ext > 0) {
			value = exclusion(hash_value - ExclusionDouble, new_depth, hash_move);
			if (value < hash_value - ExclusionDouble) singular = 2;
		} else {
		    value = exclusion(hash_value - ExclusionSingle, new_depth, hash_move);
		    if (value < hash_value - ExclusionSingle) {
			    singular = 1;
			    value = exclusion(hash_value - ExclusionDouble, new_depth, hash_move);
			    if (value < hash_value - ExclusionDouble) singular = 2;
		    }
		}
	}
skip_exclusion:
	cnt = 0;
	if (hash_move) {
		if (is_legal(move = hash_move)) {
			new_depth = Max(16, depth - 2 + Max(ext,singular));
		    do_move(move);
    		cnt++;
			if (new_depth <= 16) value = -q_search(1 - beta, new_depth, FlagNeatSearch);
    		else value = -search(1 - beta, new_depth, FlagNeatSearch | (hash_value >= beta ? FlagNullSearch : 0));
	    	if (value < -MateValue) cnt--;
	    	undo_move(move);
			if (value > curr_value) {
				curr_value = value;
    		    if (value >= beta) goto cut;
			}
		} else Current->killer[0] = 0;
	}
	
	Current->stage = 0;
	if (True(prune) && depth <= 21 && static_value + (50 + (Max(depth - 16, 0) << 4)) < beta) {
		cnt = 1;
		prune = 0;
		Current->stage = 10;
		Current->mask = Board->bb[me ^ 1];
		curr_value = static_value + (50 + (Max(depth - 16, 0) << 4));
		if (static_value + (250 + (Max(depth - 16, 0) << 5)) < beta) {
			if (Current->all_att[me] & Board->bb[BlackPawn ^ me]) {
			    Current->mask ^= Board->bb[BlackPawn ^ me];
			    curr_value = static_value + (250 + (Max(depth - 16, 0) << 5));
			}
		}
	}
	if (hash_depth >= depth) curr_value = Max(curr_value, hash_value);
	while (True(move = get_move())) {
		if (move == hash_move) continue;
		cnt++;
		ext = extension(move,0);
		check = is_check(move);
		if (Current->stage == 5 && False(check) && False(ext)) {
			if ((value = static_value + DeltaM(move) + (Max(depth - 16, 0) << 4) - (cnt << 1)) < beta && depth <= 28) {
				curr_value = Max(curr_value, value);
				continue;
			}
			if (cnt > HistoryMoveNb(depth) && False(is_defence(move, target))) continue;
		}
		new_depth = Max(16, depth - 2 + ext);
        do_move(move);
#ifndef TUNABLE
		if (depth >= 22 && (Current - 1)->stage == 5 && False(ext) && False(check) && cnt > 3) new_depth = Max(18, depth - 2 - msb(cnt));
#else
		if (depth >= 22 && True(LMR) && (Current - 1)->stage == 5 && False(ext) && False(check) && cnt > 3) new_depth = Max(18, depth - 2 - msb(cnt));
#endif
		if (new_depth <= 16) value = -q_search(1 - beta, new_depth, FlagNeatSearch);
		else value = -search(1 - beta, new_depth, FlagNeatSearch);
	    if (value >= beta && new_depth < depth - 2) {
			new_depth = depth - 2;
			if (new_depth <= 16) value = -q_search(1 - beta, new_depth, FlagNeatSearch | FlagDisableDelta | Current->killer[0]);
			else value = -search(1 - beta, new_depth, FlagNeatSearch | FlagDisableDelta | Current->killer[0]);
		}
		undo_move(move);
		if (value > curr_value) {
			curr_value = value;
		    if (value >= beta) goto cut;
		} else if (value < -MateValue) cnt--;
	}

	if (False(cnt)) {
		value = 0;
		move = 0;
		if (value > curr_value) {
			curr_value = value;
	        if (value >= beta) goto cut;
		}
	}
	hash_high(curr_value, depth);
	return curr_value;
cut:
	hash_low(move,curr_value,depth);
	Current->killer[0] = move;
	if (True(move) && False(Board->square[To(move)]) && False(move & 0xE000)) {
	    if (Current->killer[1] != move) {
		    Current->killer[2] = Current->killer[1];
	     	Current->killer[1] = move;
	    }
		HistoryGood(move,depth);
		if (Current->stage == 5) {
			for (p = Current->start; ((p + 1) < Current->current) && ((*p) & 0xFFFF) != move; p++) HistoryBad((*p) & 0xFFFF,depth);
		}
    }
	return curr_value;
}

int search_evasion(int beta, int depth, int flags) {
	int cnt, i, value, move, *p, *q, new_depth, hash_move, hash_depth, curr_value;
	
	curr_value = (Current - Data) - MateValue;
	if (flags & FlagHaltCheck) {
	    if (curr_value >= beta) return beta;
	    if (-curr_value < beta) return beta - 1;
	    halt_check;
	}
	Current->killer[0] = hash_move = flags & 0xFFFF;
	if (flags & FlagHashCheck) {
	    hash_depth = 0;
	    for (i = 0, Entry = Hash + (High32(Current->key) & hash_mask); i < 4; i++, Entry++) {
		    if (Low32(Current->key) == Entry->key) {
				UpdateDate(Entry->date, date);
			    if (Entry->high_depth >= depth && Entry->high < beta) return Entry->high;
			    if (Entry->low_depth >= depth && Entry->low >= beta) return Entry->low;
			    if (Entry->move && Entry->low_depth > hash_depth) {
				    Current->killer[0] = hash_move = Entry->move;
				    hash_depth = Entry->low_depth;
			    }
		    }
	    }
	}
	if (flags & FlagCallEvaluation) get_attacks();
	if (Board->bb[BlackKing ^ Current->turn] & Current->all_att[Current->turn]) return MateValue + 1;
	new_depth = depth;
	cnt = 0;
	if (hash_move) {
		if (is_legal(hash_move)) {
			do_move(hash_move);
			cnt++;
			if (new_depth <= 16) value = -q_search(1 - beta, new_depth, FlagNeatSearch | FlagDisableDelta);
		    else value = -search(1 - beta, new_depth, FlagNeatSearch | FlagDisableDelta);
			undo_move(hash_move);
			if (value > curr_value) {
				curr_value = value;
			    if (value >= beta) {
				    hash_low(hash_move,value,depth);
			        return value;
			    }
			}
		} else Current->killer[0] = 0;
	}
	gen_evasions(Current->moves);
	for (p = Current->moves; *p != 0; p++) {
		move = *p;
		for (q = p + 1; *q != 0; q++) {
			if (*q > *p) {
				*p = *q;
				*q = move;
				move = *p;
			}
		}
        move &= 0xFFFF;
		if (move == hash_move) continue;
		new_depth = depth;
		if (depth >= 22 && cnt >= 1) new_depth = Max(20, depth - 2 - cnt);
		do_move(move);
		cnt++;
		if (new_depth <= 16) value = -q_search(1 - beta, new_depth, FlagNeatSearch | FlagDisableDelta);
		else value = -search(1 - beta, new_depth, FlagNeatSearch | FlagDisableDelta);
		if (value >= beta && new_depth < depth) {
			new_depth = depth;
			if (new_depth <= 16) value = -q_search(1 - beta, new_depth, FlagNeatSearch | FlagDisableDelta);
		    else value = -search(1 - beta, new_depth, FlagNeatSearch | FlagDisableDelta);
		}
		undo_move(move);
		if (value > curr_value) {
			curr_value = value;
		    if (value >= beta) {
			    hash_low(move,value,depth);
			    Current->killer[0] = move;
			    return value;
		    }
		}
	}
	
	hash_high(curr_value, depth);
	return curr_value;
}

int exclusion(int beta, int depth, int killer) {
	int move, check, value;

	if (Current->all_att[Current->turn ^ 1] & Board->bb[WhiteKing | Current->turn]) check = 1;
	else check = 0;

	if (check) Current->stage = 8;
	else Current->stage = 0;

	while (True(move = get_move())) {
		if (move == killer) continue;
		do_move(move);
		if (depth <= 16) value = -q_search(1 - beta, depth, FlagNeatSearch);
		else value = -search(1 - beta, depth, FlagNeatSearch);
		undo_move(move);
		if (value >= beta) return beta;
	}
	return beta - 1;
}

int qpv_search(int alpha, int beta, int depth) {
	int i, move, me, hash_move, hash_depth, value, *p, *q, new_depth, check, old_alpha = alpha, cnt = 0;
	uint64 target;

	if ((Current - Data) - MateValue >= beta) return beta;
    if (MateValue - (Current - Data) <= alpha) return alpha;
	halt_check;
	if ((Current - Data) - MateValue > alpha) alpha = Convert(Current - Data,int) - MateValue;
	Current->killer[0] = hash_move = 0;
	hash_depth = 0;
	for (i = 0, Entry = Hash + (High32(Current->key) & hash_mask); i < 4; i++, Entry++) {
		if (Low32(Current->key) == Entry->key) {
			if (Entry->move && Entry->low_depth > hash_depth) {
				Current->killer[0] = hash_move = Entry->move;
				hash_depth = Entry->low_depth;
			}
		}
	}
	evaluate();
	if (False(Current->capture) && False(Current->move & 0xE000) && True(Current->move)) UpdateDelta;
	me = Current->turn;
	if (Board->bb[BlackKing ^ me] & Current->all_att[me]) return MateValue + 1;
	if (Board->bb[WhiteKing | me] & Current->all_att[me ^ 1]) check = 1;
	else check = 0;
	if (Current->score > 0) Current->score += 5;
	else Current->score = Min(0, Current->score + 5);
	if (False(check)) {
		if (Current->score >= beta) return beta;
		if (Current->score > alpha) alpha = Current->score;
		if (False(Board->square[To(hash_move)])) hash_move = 0;
		new_depth = depth - 2;
	} else new_depth = depth - 1;
	if (new_depth < 1) new_depth = 1;
	if (True(hash_move)) {
		if (is_legal(hash_move)) {
		    do_move(hash_move);
		    cnt++;
		    value = -qpv_search(-beta, -alpha, new_depth);
		    undo_move(hash_move);
			if (value < -MateValue) cnt--;
		    if (value > alpha) {
			    alpha = value;
			    if (value >= beta) {
			        hash_low_pv(hash_move,beta,16);
			        return beta;
			    }
		    }
		} else Current->killer[0] = 0;
	}

	target = Board->bb[Black ^ me];
	if (check) gen_evasions(Current->moves);
	else {
		if (Current->score < alpha - 250) {
		    target ^= Board->bb[BlackPawn ^ me];
		    if (Current->score < alpha - 450) {
			    target ^= Board->bb[BlackKnight ^ me] | Board->bb[BlackLight ^ me] | Board->bb[BlackDark ^ me];
			    if (Current->score < alpha - 650) target ^= Board->bb[BlackRook ^ me];
		    }
	    }
	    gen_captures(target,Current->moves);
	}
	for (p = Current->moves; *p != 0; p++) {
		move = *p;
		for (q = p + 1; *q != 0; q++) {
		    if (*q > *p) {
		        *p = *q;
		    	*q = move;
			    move = *p;
			}
		}
        move &= 0xFFFF;
		if (move == hash_move) continue;
		do_move(move);
		cnt++;
		if (cnt == 1) {
			value = -qpv_search(-beta,-alpha,new_depth);
		} else {
			value = -q_search(-alpha, new_depth, FlagNeatSearch);
			if (value > alpha) value = -qpv_search(-beta,-alpha,new_depth);
		}
		undo_move(move);
		if (value < -MateValue) cnt--;
		if (value > alpha) {
			alpha = value;
			Current->killer[0] = move;
			if (value >= beta) {
			    hash_low_pv(move,beta,16);
		  	    return beta;
			}
		}
	}

	if (depth >= 4 && False(check)) {
		gen_checks(Board->bb[Black ^ me] ^ target,Current->moves);
		for (p = Current->moves; *p != 0; p++) {
		    move = *p;
		    for (q = p + 1; *q != 0; q++) {
		        if (*q > *p) {
		            *p = *q;
		    	    *q = move;
			        move = *p;
			    }
		    }
            move &= 0xFFFF;
			if (move == hash_move) continue;
			do_move(move);
			evaluate();
			if (Board->bb[WhiteKing | me] & Current->all_att[me ^ 1]) {
				undo_move(move);
				continue;
			}
			cnt++;
		    if (cnt == 1) {
			    value = -qpv_search(-beta,-alpha,new_depth);
		    } else {
			    value = -q_evasion(-alpha, new_depth, FlagNeatSearch);
			    if (value > alpha) value = -qpv_search(-beta,-alpha,new_depth);
		    }
			undo_move(move);
			if (value > alpha) {
			    alpha = value;
			    Current->killer[0] = move;
			    if (value >= beta) {
			        hash_low_pv(move,beta,16);
		  	        return beta;
			    }
		    }
		}
	}

	if (depth >= 15 && cnt <= 3 && False(check) && Current->score > beta - 30) {
		Current->margin = beta - Current->score;
		gen_delta_moves(Current->moves);
		for (p = Current->moves; *p != 0; p++) {
		    move = *p;
		    for (q = p + 1; *q != 0; q++) {
			    if (*q > *p) {
				    *p = *q;
				    *q = move;
				    move = *p;
			    }
		    }
            move &= 0xFFFF;
			if (move == hash_move) continue;
			do_move(move);
			if (cnt == 1) {
			    value = -qpv_search(-beta,-alpha,new_depth);
		    } else {
			    value = -q_search(-alpha, new_depth, FlagNeatSearch);
			    if (value > alpha) value = -qpv_search(-beta,-alpha,new_depth);
		    }
			undo_move(move);
			if (value > alpha) {
			    alpha = value;
			    Current->killer[0] = move;
			    if (value >= beta) {
			        hash_low_pv(move,beta,16);
		  	        return beta;
			    }
		    }
			if (cnt >= 6) break;
		}
	}

	if (True(check) && False(cnt)) {
		value = Convert(Current - Data,int) - MateValue;
		if (value > alpha) {
			alpha = value;
	        if (value >= beta) {
				hash_low_pv(move,beta,16);
		  	    return beta;
			}
		} 
	}

	if (alpha > old_alpha) hash_low_pv(Current->killer[0],alpha,16);
	hash_high_pv(alpha,16);
	return alpha;
}

int pv_search(int alpha, int beta, int depth) {
	int i, cnt, move, me, ext, hash_move, base_depth, singular, new_depth, hash_depth, hash_value = -MateValue, value, check, old_alpha = alpha;

	if ((Current - Data) - MateValue >= beta) return beta;
	if (MateValue - (Current - Data) <= alpha) return alpha;
	halt_check;
	if ((Current - Data) - MateValue > alpha) alpha = Convert(Current - Data,int) - MateValue;
	Current->killer[0] = hash_move = 0;
	hash_depth = 0;
#ifdef TUNABLE
	for (i = 0, Entry = Hash + (High32(Current->key) & hash_mask); i < 4; i++, Entry++) {
		if (Low32(Current->key) == Entry->key) {
			if (Entry->low_depth == 127) {
				alpha = Max(alpha, Entry->low);
				if (alpha >= beta) return beta;
			} else if (Entry->high_depth == 127) {
				beta = Min(beta, Entry->high);
				if (alpha >= beta) return alpha;
			}
		}
	}
#endif
#ifndef KNS_TESTING
#ifndef TUNABLE
	if (False(Infinite) || depth <= 24) {
#else
	if (False(Infinite) || depth <= 24 || True(PVHashing)) {
#endif
#else
	if (1) {
#endif
	    for (i = 0, PVEntry = PVHash + (High32(Current->key) & pv_hash_mask); i < pv_cluster_size; i++, PVEntry++) {
		    if (PVEntry->key == Low32(Current->key)) {
			    PVEntry->date = date;
			    if (PVEntry->depth >= depth) {
				    if (PVEntry->move) Current->killer[0] = PVEntry->move;
				    return PVEntry->value;
			    }
		    }
	    }
	}
	for (i = 0, Entry = Hash + (High32(Current->key) & hash_mask); i < 4; i++, Entry++) {
		if (Low32(Current->key) == Entry->key) {
			UpdateDate(Entry->date, date);
		    if (Entry->move && Entry->low_depth > hash_depth) {
				Current->killer[0] = hash_move = Entry->move;
				hash_depth = Entry->low_depth;
				hash_value = Entry->low;
			}
		}
	}
	evaluate();
	if (False(Current->capture) && False(Current->move & 0xE000) && True(Current->move)) UpdateDelta;
	if ((False(hash_move) || hash_depth < depth - 4) && depth >= 19) {
		value = pv_search(alpha, beta, depth - 3);
		if (value <= alpha) {
			value = search(alpha - ExclusionSingle, depth - 4, hash_move | FlagHashCheck);
			if (value >= alpha - ExclusionSingle) {
				hash_move = Current->killer[0];
			    hash_value = value;
			    hash_depth = depth - 4;
			} else if (depth >= 20 && alpha > -EvalValue) {
				value = search(alpha - ExclusionDouble, depth - 4, hash_move | FlagHashCheck);
				if (value >= alpha - ExclusionDouble) {
					hash_move = Current->killer[0];
			        hash_value = value;
			        hash_depth = depth - 4;
				} else if (depth >= 21) {
					value = search(alpha - (2 * ExclusionDouble), depth - 4, hash_move | FlagHashCheck);
				    if (value >= alpha - (2 * ExclusionDouble)) {
					    hash_move = Current->killer[0];
			            hash_value = value;
			            hash_depth = depth - 4;
				    }
				}
			}
		} else {
			hash_move = Current->killer[0];
			hash_value = value;
			hash_depth = depth - 3;
		}
	} else if (depth > 24) {
		value = pv_search(alpha, beta, depth - 4);
		if (value > alpha) {
			hash_value = Max(hash_value, value);
			hash_move = Current->killer[0];
		}
	}

	me = Current->turn;
	cnt = 0;
	if (Board->bb[BlackKing ^ me] & Current->all_att[me]) return MateValue + 1;
	if (Board->bb[WhiteKing | me] & Current->all_att[me ^ 1]) check = 1;
	else check = 0;
	if (check) base_depth = depth;
	else base_depth = depth - 2;

	singular = 0;
	ext = extension(hash_move,1);
	if (ext > 1) goto skip_exclusion;
	if (depth >= 28 && True(hash_move) && False(check) && is_legal(hash_move) && hash_value > -EvalValue && beta > -EvalValue && alpha < EvalValue) {
		new_depth = Max(depth - 12,18);
		if (hash_value < alpha) {
			do_move(hash_move);
			value = -pv_search(-beta, -alpha, new_depth);
			undo_move(hash_move);
			if (value > alpha) {
				hash_value = value;
				hash_depth = new_depth;
			}
		}
		if (ext > 0) {
		    value = exclusion(hash_value - ExclusionDouble, new_depth, hash_move);
			if (value < hash_value - ExclusionDouble) singular = 2;	
		} else {
		    value = exclusion(hash_value - ExclusionSingle, new_depth, hash_move);
		    if (value < hash_value - ExclusionSingle) {
			    singular = 1;
			    value = exclusion(hash_value - ExclusionDouble, new_depth, hash_move);
			    if (value < hash_value - ExclusionDouble) singular = 2;
		    }
		}
	}
	skip_exclusion:
	if (hash_move) {
		if (is_legal(move = hash_move)) {
	        new_depth = Min(depth,base_depth + Max(ext,singular));
			do_move(move);
			cnt++;
			if (new_depth <= 16) value = -qpv_search(-beta, -alpha, new_depth);
			else value = -pv_search(-beta, -alpha, new_depth);
			if (value < -MateValue) cnt--;
			undo_move(move);
			if (value > alpha) {
				alpha = value;
				Current->killer[0] = move;
				if (value >= beta) goto cut;
			}
		} else Current->killer[0] = 0;
	}

	if (check) Current->stage = 8;
	else Current->stage = 0;
    while (True(move = get_move())) {
		if (move == hash_move) continue;
	    new_depth = Min(depth,base_depth + extension(move,1));
        do_move(move);
		cnt++;
		if (cnt == 1) {
			if (new_depth <= 16) value = -qpv_search(-beta, -alpha, new_depth);
			else value = -pv_search(-beta, -alpha, new_depth);
		} else {
		    if (new_depth <= 16) value = -q_search(-alpha, new_depth, FlagNeatSearch);
		    else value = -search(-alpha, new_depth, FlagNeatSearch);
		    if (value > alpha) {
				if (new_depth <= 16) value = -qpv_search(-beta, -alpha, new_depth);
				else value = -pv_search(-beta, -alpha, new_depth);
			}
		}
		if (value < -MateValue) cnt--;
		undo_move(move);
		if (value > alpha) {
			alpha = value;
			Current->killer[0] = move;
		    if (value >= beta) goto cut;
		}
    }
	if (False(cnt)) {
		move = 0;
		if (check) value = Convert(Current - Data,int) - MateValue;
		else value = 0;
		if (value > alpha) {
			alpha = value;
	        if (value >= beta) goto cut;
		}
	}
	if (alpha > old_alpha) {
		hash_low_pv(Current->killer[0],alpha,depth);
		hash_exact(Current->killer[0],alpha,depth);
	}
	hash_high_pv(alpha, depth);
	return alpha;
cut:
	hash_low_pv(move,beta,depth);
	Current->killer[0] = move;
	if (move && False(Board->square[To(move)]) && False(move & 0xE000)) {
	    if (Current->killer[1] != move) {
		    Current->killer[2] = Current->killer[1];
	     	Current->killer[1] = move;
	    }
    }
	return beta;
}

void root() {
	int i, time, depth, value, previous = -MateValue, alpha, beta, delta;

	date++;
	nodes = check_node = 0;
	memcpy(Data,Current,sizeof(GData));
	memcpy(SaveData,Current,sizeof(GData));
	memcpy(SaveBoard,Board,sizeof(GBoard));
	Current = Data;
	Easy = 1;
	evaluate();
	gen_legal_moves();
	if (PVN > 1) {
		memset(MultiPV,0,128 * sizeof(int));
		for (i = 0; Current->moves[i] != 0; i++) MultiPV[i] = Current->moves[i];
	}
	best_move = Current->moves[0];
	if (False(best_move)) return;
	if (False(Infinite) && False(Current->moves[1])) {
		Current->score = value = root_search(-MateValue, MateValue, 18);
		LastDepth = 128;
		send_pv(6, -MateValue, MateValue, value);
		send_best_move();
		Searching = 0;
		return;
	}

	if (False(Infinite) && LastDepth < 128) {
		for (i = 0, PVEntry = PVHash + (High32(Current->key) & pv_hash_mask); i < pv_cluster_size; i++, PVEntry++) {
			if (PVEntry->key == Low32(Current->key)) {
				if (PVEntry->depth >= LastDepth - 8) {
					if (Easy) time = (TimeLimit1 * EasyMargin)/NormalMargin;
					else time = TimeLimit1;
				    for (depth = PVEntry->depth; depth < LastDepth - 1; depth += 2) time *= 2;
					if (depth < LastDepth) time = (time * 7)/5;
				    if (time < LastTime) {
					    LastDepth = 128;
					    LastTime = 0;
					    send_pv(OutputDepth(PVEntry->depth), -MateValue, MateValue, PVEntry->value);
					    send_best_move();
						Searching = 0;
					    return;
				    }
				}
				break;
			}
		}
	}

	if (setjmp(Jump)) {
		Searching = 0;
		Current = Data;
		memcpy(Board,SaveBoard,sizeof(GBoard));
	    memcpy(Current,SaveData,sizeof(GData));
		send_best_move();
		return;
	}
	Bad = 0;
	Change = 0;
#ifndef TUNABLE
	for (depth = 18; depth < DepthLimit; depth += 2) {
#else
	for (depth = 18; depth < DepthLimit; depth += (HalfPlySearch ? 1 : 2)) {
#endif
		memset(Data + 1, 0, 127 * sizeof(GData));
#ifndef ASPIRATION
		if (PVN > 1) value = multipv(depth);
		else Current->score = value = root_search(-MateValue, MateValue, depth);
#else
		if (PVN > 1) value = multipv(depth);
#ifndef TUNABLE
		else if (OutputDepth(depth) < 7) Current->score = value = root_search(-MateValue, MateValue, depth);
#else
		else if (OutputDepth(depth) < 7 || False(Aspiration)) Current->score = value = root_search(-MateValue, MateValue, depth);
#endif
		else {
			delta = ExclusionSingle;
			alpha = previous - delta;
			beta = previous + delta;
loop:
			if (delta >= ExclusionSingle * 32) {
				Current->score = value = root_search(-MateValue, MateValue, depth);
				goto finish;
			}
			Current->score = value = root_search(alpha, beta, depth);
			if (value <= alpha) {
#ifdef TUNABLE
				if (OutputDepth(depth) > 5 && True(ExtendedInfo)) send_pv(OutputDepth(depth),alpha,beta,value);
#endif
				alpha -= delta;
				delta *= 2;
				goto loop;
			} else if (value >= beta) {
				if (OutputDepth(depth) > 8) send_pv(OutputDepth(depth),alpha,beta,value);
				beta += delta;
				delta *= 2;
				LastDepth = depth;
				LastTime = GetTickCount() - StartTime;
				if (depth < DepthLimit) depth += 2;
				check_time(0);
				memset(Data + 1, 0, 127 * sizeof(GData));
				goto loop;
			}
		}
finish:
#endif
		if (OutputDepth(depth) > 8 && PVN == 1) send_pv(OutputDepth(depth),-MateValue,MateValue,value);
		if (value < previous - 50) Bad = 1;
		else Bad = 0;
		LastDepth = depth;
		LastTime = GetTickCount() - StartTime;
		previous = value;
		check_time(0);
	}
	Searching = 0;
	if (False(Infinite) || DepthLimit < 128) send_best_move();
}

int root_search(int alpha, int beta, int depth) {
	int *p, *q, *r, move, value, old_alpha = alpha, cnt, new_depth, old_best, fail_low = 0;
	Early = 1;
	Current->moves[0] |= 1 << 21;
#ifndef KNS_TESTING
	if (True(Print))
#endif
	fprintf(stdout,"info depth %d\n",OutputDepth(depth)); fflush(stdout);
	for (p = Current->moves, cnt = 1; True(move = ((*p) & 0xFFFF)); p++, cnt++) {
		move_to_string(move,score_string);
		if (True(Print)) sprintf(info_string,"info currmove %s currmovenumber %d\n",score_string,cnt);
		new_depth = depth - 2 + extension(move, 1);
		do_move(move);
		if (p == (Current - 1)->moves) {
			value = -pv_search(-beta,-alpha,new_depth);
			if (value <= alpha) fail_low = 1;
		} else {
			value = -search(-alpha,new_depth,FlagNeatSearch);
			if (value > alpha) {
				*p |= 0x00010000;
				Early = 0;
				if (False(fail_low)) Easy = 0;
				old_best = best_move;
				best_move = move;
				value = -pv_search(-beta,-alpha,new_depth);
				if (value <= alpha) best_move = old_best;
				if (value > alpha && depth >= 28) Change = 1;
			}
		}
		undo_move(move);
		if (value > alpha) {
			alpha = value;
			best_move = move;
			best_score = value;
			*p |= cnt << 21;
			hash_low_pv(best_move,value,depth);
			if (value >= beta) goto cut;
		}
	}
	if (alpha > old_alpha) hash_low_pv(best_move,alpha,depth);
	hash_high_pv(alpha,depth);
	for (p = Current->moves; True(*p); p++) {
		for (q = p + 1; True(*q); q++) {
			if (((*q) & 0xFFFF0000) > ((*p) & 0xFFFF0000)) {
				move = *p;
				*p = *q;
				for (r = q; r > p + 1; r--) {
					*r = *(r - 1);
				}
				*(p + 1) = move;
			}
		}
	}
	for (p = Current->moves; True(*p); p++) *p &= 0xFFFF;
	return alpha;
cut:
	hash_low_pv(best_move,beta,depth);
	for (q = p + 1; True(*q); q++) *q &= 0xFFFF; 
	for (p = Current->moves; True(*p); p++) {
		for (q = p + 1; True(*q); q++) {
			if (((*q) & 0xFFFF0000) > ((*p) & 0xFFFF0000)) {
				move = *p;
				*p = *q;
				for (r = q; r > p + 1; r--) {
					*r = *(r - 1);
				}
				*(p + 1) = move;
			}
		}
	}
	for (p = Current->moves; True(*p); p++) *p &= 0xFFFF;
	return beta;
}

int multipv(int depth) {
	int move, low = MateValue, value, i, cnt, new_depth = depth;
	fprintf(stdout,"info depth %d\n",OutputDepth(depth)); fflush(stdout);
	for (cnt = 0; cnt < PVN && True(move = (MultiPV[cnt] & 0xFFFF)); cnt++) {
		MultiPV[cnt] = move;
		move_to_string(move,score_string);
		if (True(Print)) sprintf(info_string,"info currmove %s currmovenumber %d\n",score_string,cnt + 1);
		new_depth = depth - 2 + extension(move, 1);
		do_move(move);
		value = -pv_search(-MateValue,MateValue,new_depth);
		MultiPV[cnt] |= value << 16;
		if (value < low) low = value;
		undo_move(move);
		for (i = cnt - 1; i >= 0; i--) {
			if ((MultiPV[i] >> 16) < value) {
				MultiPV[i + 1] = MultiPV[i];
				MultiPV[i] = move | (value << 16);
			}
		}
		best_move = MultiPV[0] & 0xFFFF;
		Current->score = MultiPV[0] >> 16;
		send_multipv(OutputDepth(depth), cnt);
	}
	for (;True(move = (MultiPV[cnt] & 0xFFFF)); cnt++) {
		MultiPV[cnt] = move;
		move_to_string(move,score_string);
		if (True(Print)) sprintf(info_string,"info currmove %s currmovenumber %d\n",score_string,cnt + 1);
		new_depth = depth - 2 + extension(move, 1);
		do_move(move);
		value = -search(-low, new_depth, FlagNeatSearch);
		if (value > low) value = -pv_search(-MateValue,-low,new_depth);
		MultiPV[cnt] |= value << 16;
		undo_move(move);
		if (value > low) {
			for (i = cnt; i >= PVN; i--) MultiPV[i] = MultiPV[i - 1];
			MultiPV[PVN - 1] = move | (value << 16);
			for (i = PVN - 2; i >= 0; i--) {
				if ((MultiPV[i] >> 16) < value) {
					MultiPV[i + 1] = MultiPV[i];
					MultiPV[i] = move | (value << 16);
				}
			}
			best_move = MultiPV[0] & 0xFFFF;
		    Current->score = MultiPV[0] >> 16;
			low = MultiPV[PVN - 1] >> 16;
			send_multipv(OutputDepth(depth), cnt);
		}
	}
	return Current->score;
}

void send_pv(int depth, int alpha, int beta, int score) {
	int i, pos, move, sel_depth;
	sint64 nps;
	if (False(Print)) return;
	for (sel_depth = 1; sel_depth < 127 && True((Data + sel_depth)->key); sel_depth++);
	sel_depth--;
	pv_length = 64;
	if (False(move = Current->moves[0])) return;
	PV[0] = move;
	do_move(move);
	pvp = 1;
	pick_pv();
	undo_move(move);
	pos = 0;
	for (i = 0; i < 64 && True(PV[i]); i++) {
		if (pos > 0) { 
			pv_string[pos] = ' '; 
			pos++; 
		}
        move = PV[i];
        pv_string[pos++] = ((move >> 6) & 7) + 'a';
        pv_string[pos++] = ((move >> 9) & 7) + '1';
        pv_string[pos++] = (move & 7) + 'a';
        pv_string[pos++] = ((move >> 3) & 7) + '1';
        if (IsPromotion(move)) {
            if ((move & 0xF000) == FlagPQueen)  pv_string[pos++] = 'q';
            else if ((move & 0xF000) == FlagPRook)   pv_string[pos++] = 'r';
            else if ((move & 0xF000) == FlagPLight || (move & 0xF000) == FlagPDark) pv_string[pos++] = 'b';
            else if ((move & 0xF000) == FlagPKnight) pv_string[pos++] = 'n';
		}
        pv_string[pos] = 0;
	}
	score_string[0] = 'c';
	score_string[1] = 'p';
    if (score > EvalValue) {
		strcpy(score_string,"mate ");
		score = (MateValue - score + 1)/2;
	    score_string[6] = 0;
	} else if (score < -EvalValue) {
        strcpy(score_string,"mate ");
		score = -(score + MateValue + 1)/2;
		score_string[6] = 0;
	} else {
        score_string[0] = 'c';
	    score_string[1] = 'p';
		score_string[2] = ' ';
		score_string[3] = 0;
	}
	nps = GetTickCount() - StartTime;
	if (nps) nps = (nodes * 1000)/nps; 
	if (score < beta) {
		if (score <= alpha) fprintf(stdout,"info depth %d seldepth %d score %s%d upperbound nodes %I64d nps %I64d pv %s\n",depth,sel_depth,score_string,score,nodes,nps,pv_string);
		else fprintf(stdout,"info depth %d seldepth %d score %s%d nodes %I64d nps %I64d pv %s\n",depth,sel_depth,score_string,score,nodes,nps,pv_string);
	} else fprintf(stdout,"info depth %d seldepth %d score %s%d lowerbound nodes %I64d nps %I64d pv %s\n",depth,sel_depth,score_string,score,nodes,nps,pv_string);
	fflush(stdout);
}

void send_multipv(int depth, int curr_number) {
	int i, j, pos, move, score;
	sint64 nps;
	if (False(Print)) return;
	for (j = 0; j < PVN && True(MultiPV[j]); j++) {
		pv_length = 63;
		pvp = 0;
		move = MultiPV[j] & 0xFFFF;
		score = MultiPV[j] >> 16;
		memset(PV,0,64 * sizeof(uint16));
		do_move(move);
		pick_pv();
		undo_move(move);
		for (i = 63; i > 0; i--) PV[i] = PV[i - 1];
		PV[0] = move;
		pos = 0;
		for (i = 0; i < 64 && True(PV[i]); i++) {
			if (pos > 0) { 
				pv_string[pos] = ' '; 
				pos++; 
			}
        	move = PV[i];
        	pv_string[pos++] = ((move >> 6) & 7) + 'a';
        	pv_string[pos++] = ((move >> 9) & 7) + '1';
        	pv_string[pos++] = (move & 7) + 'a';
        	pv_string[pos++] = ((move >> 3) & 7) + '1';
        	if (IsPromotion(move)) {
            	if ((move & 0xF000) == FlagPQueen)  pv_string[pos++] = 'q';
            	else if ((move & 0xF000) == FlagPRook)   pv_string[pos++] = 'r';
            	else if ((move & 0xF000) == FlagPLight || (move & 0xF000) == FlagPDark) pv_string[pos++] = 'b';
            	else if ((move & 0xF000) == FlagPKnight) pv_string[pos++] = 'n';
			}
        	pv_string[pos] = 0;
		}
		score_string[0] = 'c';
		score_string[1] = 'p';
		if (score > EvalValue) {
			strcpy(score_string,"mate ");
			score = (MateValue - score + 1)/2;
	    	score_string[6] = 0;
		} else if (score < -EvalValue) {
        	strcpy(score_string,"mate ");
			score = -(score + MateValue + 1)/2;
			score_string[6] = 0;
		} else {
        	score_string[0] = 'c';
	    	score_string[1] = 'p';
			score_string[2] = ' ';
			score_string[3] = 0;
		}
		nps = GetTickCount() - StartTime;
	    if (nps) nps = (nodes * 1000)/nps;
		fprintf(stdout,"info multipv %d depth %d score %s%d nodes %I64d nps %I64d pv %s\n",j + 1,(j <= curr_number ? depth : depth - 1),score_string,score,nodes,nps,pv_string);
		fflush(stdout);
	}
}

void send_best_move() {
	int ponder;
#ifndef KNS_TESTING
	if (False(Print)) return;
#endif
	fprintf(stdout,"info nodes %I64d score cp %d\n",nodes,best_score);
	if (!best_move) return;
	Current = Data;
	do_move(best_move);
	pv_length = 1;
	pvp = 0;
	pick_pv();
	ponder = PV[0];
	if (True(ponder) && False(is_legal(ponder))) ponder = 0;
	undo_move(best_move);
	move_to_string(best_move,pv_string);
	if (ponder) {
		move_to_string(ponder,score_string);
		fprintf(stdout,"bestmove %s ponder %s\n",pv_string,score_string);
	} else fprintf(stdout,"bestmove %s\n",pv_string);
	fflush(stdout);
}

void get_position(char string[]) {
	const char * fen;
    char * moves;
    const char * ptr;
    int move;

    fen = strstr(string,"fen ");
    moves = strstr(string,"moves ");
    if (fen != NULL) init_board(fen+4);
    else init_board(StartPos);
    if (moves != NULL) {
        ptr = moves+6;
        while (*ptr != 0) {
            pv_string[0] = *ptr++;
            pv_string[1] = *ptr++;
            pv_string[2] = *ptr++;
            pv_string[3] = *ptr++;
            if (*ptr == 0 || *ptr == ' ') pv_string[4] = 0;
            else { 
				pv_string[4] = *ptr++; 
				pv_string[5] = 0; 
			}
            move = move_from_string(pv_string);
            do_move(move);
			memcpy(Data,Current,sizeof(GData));
			Current = Data;
            while (*ptr == ' ') ptr++;
        }
    }
}

void get_time_limit(char string[]) {
	const char * ptr;
	int i, time, inc, wtime, btime, winc, binc, moves, pondering, movetime = 0;
  
	Infinite = 1;
	MoveTime = 0;
	SearchMoves = 0;
	SMPointer = 0;
	pondering = 0;
	TimeLimit1 = 0;
	TimeLimit2 = 0;
	wtime = btime = 0;
	winc = binc = 0;
	moves = 0;
	Stop = 0;
	DepthLimit = 128;
    ptr = strtok(string," ");
    for (ptr = strtok(NULL," "); ptr != NULL; ptr = strtok(NULL," ")) {
		if (!strcmp(ptr,"binc")) {
			ptr = strtok(NULL," "); 
			binc = atoi(ptr);
			Infinite = 0;
		} else if (!strcmp(ptr,"btime")) { 
			ptr = strtok(NULL," "); 
			btime = atoi(ptr);
			Infinite = 0;
		} else if (!strcmp(ptr,"depth")) { 
			ptr = strtok(NULL," "); 
			DepthLimit = 16 + 2 * atoi(ptr) + 1; 
			Infinite = 1;
		} else if (!strcmp(ptr,"infinite")) { 
			Infinite = 1; 
		} else if (!strcmp(ptr,"movestogo")) { 
			ptr = strtok(NULL," "); 
			moves = atoi(ptr);
			Infinite = 0;
		} else if (!strcmp(ptr,"winc")) { 
			ptr = strtok(NULL," "); 
			winc = atoi(ptr);
			Infinite = 0;
		} else if (!strcmp(ptr,"wtime")) { 
			ptr = strtok(NULL," "); 
			wtime = atoi(ptr); 
			Infinite = 0;
		} else if (!strcmp(ptr,"movetime")) { 
			ptr = strtok(NULL," "); 
			movetime = atoi(ptr);
			MoveTime = 1;
			Infinite = 0;
		} else if (!strcmp(ptr,"searchmoves")) {
			if (False(SearchMoves)) {
				for (i = 0; i < 256; i++) SMoves[i] = 0;
			}
		    SearchMoves = 1;
		    ptr += 12;
			while (ptr != NULL && ptr[0] >= 'a' && ptr[0] <= 'h' && ptr[1] >= '1' && ptr[1] <= '8') {
				pv_string[0] = *ptr++;
                pv_string[1] = *ptr++;
                pv_string[2] = *ptr++;
                pv_string[3] = *ptr++;
                if (*ptr == 0 || *ptr == ' ') pv_string[4] = 0;
                else { 
				    pv_string[4] = *ptr++; 
				    pv_string[5] = 0; 
			    }
				SMoves[SMPointer] = move_from_string(pv_string);
				SMPointer++;
				ptr = strtok(NULL," ");
			}
		} else if (!strcmp(ptr,"ponder")) pondering = 1;
    }
	if (pondering) Infinite = 1;
	if (Current->turn == White) {
		time = wtime;
		inc = winc;
	} else {
		time = btime;
		inc = binc;
	}
	time = time - Min(time/2,1000);
	TimeLimit2 = time/3;
	if (moves > 30 && moves <= 60) moves = 30; // for testing purposes
	if (moves) {
		moves = Max(moves - 1,1);
		TimeLimit1 = ((time + (inc * moves)) * 30)/(moves * TimeValue);
	} else TimeLimit1 = (time + inc * 30)/TimeValue;
	if (Ponder) TimeLimit1 = (TimeLimit1 * PonderRatio)/16;
	if (MoveTime) {
		TimeLimit2 = movetime;
		TimeLimit1 = TimeLimit2 * 100;
	}
    InfoTime = StartTime = GetTickCount();
	Searching = 1;
	if (False(Infinite)) PVN = 1;
	root();
}

void check_time(int searching) {
	while (!Stop && input()) uci();
	if (Stop) goto jump;
	CurrTime = GetTickCount();
	int Time = Convert(CurrTime - StartTime,int);
	if (True(Print) && Time > InfoLag && CurrTime - InfoTime > InfoDelay) {
		InfoTime = CurrTime;
		if (info_string[0]) {
			fprintf(stdout,"%s",info_string);
			info_string[0] = 0;
			fflush(stdout);
		}
	}
	if (True(Infinite)) return;
	if (Time > TimeLimit2) goto jump;
	if (False(searching)) {
		if (2 * Time > TimeLimit2 && False(MoveTime)) goto jump;
		if (False(Bad)) {
			if (Time > TimeLimit1) goto jump;
			if (False(Change) && Time * 16 > TimeLimit1 * NormalMargin) goto jump;
			if (True(Early) && Time * 16 > TimeLimit1 * EarlyMargin) goto jump;
			if (True(Easy) && Time * 16 > TimeLimit1 * EasyMargin) goto jump;
		}
#ifdef KNS_TESTING
		if (True(NodeLimit) && nodes >= NodeLimit) goto jump;
#endif
	}
	return;
jump:
	Stop = 1;
	longjmp(Jump,1);
}

void init_search() {
	int i, j;
	memset(Hash,0,hashSize * sizeof(GEntry));
	for (i = 0; i < 16; i++)
		for (j = 0; j < 64; j++) {
			History[i][j] = HistoryLoss(1);
			Delta[i][j] = 0;
		}
	init_board(StartPos);
	PVN = 1;
	Infinite = 1;
	SearchMoves = 0;
	TimeLimit1 = TimeLimit2 = 0;
	Stop = Searching = 0;
	DepthLimit = 128;
	LastDepth = 128;
	LastTime = 0;
	Print = 1;
}

int input() {
    DWORD p;

	if (False(Console)) {
	    if (PeekNamedPipe(StreamHandle,NULL,0,NULL,&p,NULL)) return (p > 0);
        else return 1;
	} else {
		GetNumberOfConsoleInputEvents(StreamHandle,&p);
        return (p > 1);
	}
}

void bench(char string[]) {
	const char * ptr;
	int depth;
	uint64 Time, delta;
#ifdef CPU_TIMING
	FILETIME ftCreation, ftExit, ftKernel, ftUser;
#endif
	ptr = string + 6;
	depth = atoi(ptr);
	if (depth < 5) depth = 5;
	if (depth > 32) depth = 32;
	depth = 16 + depth * 2;
	init_search();
#ifndef CPU_TIMING
	Time = GetTickCount();
#else
	GetProcessTimes(CurrentHandle,&ftCreation,&ftExit,&ftKernel,&ftUser);
	Time = *(uint64*)(&ftKernel) + *(uint64*)(&ftUser);
#endif
	Infinite = 1;
	nodes = 0;
	init_board(B1); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 1, nodes = %I64d\n",nodes); date++;
	init_board(B2); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 2, nodes = %I64d\n",nodes); date++;
	init_board(B3); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 3, nodes = %I64d\n",nodes); date++;
	init_board(B4); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 4, nodes = %I64d\n",nodes); date++;
	init_board(B5); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 5, nodes = %I64d\n",nodes); date++;
	init_board(B6); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 6, nodes = %I64d\n",nodes); date++;
	init_board(B7); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 7, nodes = %I64d\n",nodes); date++;
	init_board(B8); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 8, nodes = %I64d\n",nodes); date++;
	init_board(B9); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 9, nodes = %I64d\n",nodes); date++;
	init_board(B10); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 10, nodes = %I64d\n",nodes); date++;
	init_board(B11); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 11, nodes = %I64d\n",nodes); date++;
	init_board(B12); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 12, nodes = %I64d\n",nodes); date++;
	init_board(B13); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 13, nodes = %I64d\n",nodes); date++;
	init_board(B14); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 14, nodes = %I64d\n",nodes); date++;
	init_board(B15); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 15, nodes = %I64d\n",nodes); date++;
	init_board(B16); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 16, nodes = %I64d\n",nodes); date++;
	init_board(B17); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 17, nodes = %I64d\n",nodes); date++;
	init_board(B18); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 18, nodes = %I64d\n",nodes); date++;
	init_board(B19); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 19, nodes = %I64d\n",nodes); date++;
	init_board(B20); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 20, nodes = %I64d\n",nodes); date++;
	init_board(B21); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 21, nodes = %I64d\n",nodes); date++;
	init_board(B22); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 22, nodes = %I64d\n",nodes); date++;
	init_board(B23); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 23, nodes = %I64d\n",nodes); date++;
	init_board(B24); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 24, nodes = %I64d\n",nodes); date++;
	init_board(B25); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 25, nodes = %I64d\n",nodes); date++;
	init_board(B26); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 26, nodes = %I64d\n",nodes); date++;
	init_board(B27); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 27, nodes = %I64d\n",nodes); date++;
	init_board(B28); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 28, nodes = %I64d\n",nodes); date++;
	init_board(B29); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 29, nodes = %I64d\n",nodes); date++;
	init_board(B30); pv_search(-MateValue,MateValue,depth); fprintf(stdout,"Positions parsed: 30, nodes = %I64d\n",nodes); date++;
#ifndef CPU_TIMING
	delta = GetTickCount() - Time;
#else
	GetProcessTimes(CurrentHandle,&ftCreation,&ftExit,&ftKernel,&ftUser);
	delta = (*(uint64*)(&ftKernel) + *(uint64*)(&ftUser) - Time)/Convert(10000,uint64);
#endif
	fprintf(stdout,"\nBenchmark: depth = %d\n",OutputDepth(depth));
	fprintf(stdout,"nodes = %I64d, time = %lfs, nps = %d\n",nodes,0.001*(double)(delta),(nodes * Convert(1000,uint64))/delta);
}

void uci() {
    char *ptr;
	int value;

    (void)fgets(input_string, 65536, stdin);
    if (feof(stdin)) exit(0);
    ptr = strchr(input_string, '\n');
    if (ptr != NULL) *ptr = 0;
    if (!strcmp(input_string, "uci")) {
		fprintf(stdout,"id name %s\n",NAME);
		fprintf(stdout,"option name Hash type spin min 1 max 1024 default 64\n");
		fprintf(stdout,"option name Ponder type check default false\n");
		fprintf(stdout,"option name MultiPV type spin min 1 max 64 default 1\n");
		fprintf(stdout,"option name Clear Hash type button\n");
        fprintf(stdout,"uciok\n");
		if (False(Searching)) init_search();
    } else if (!strcmp(input_string,"ucinewgame")) {
        Stop = 0;
		init_search();
    } else if (!strcmp(input_string,"isready")) {
        fprintf(stdout,"readyok\n");
		fflush(stdout);
    }  else if (!memcmp(input_string,"position",8)) {
        if (False(Searching)) get_position(input_string);
    } else if (!memcmp(input_string,"go",2)) {
        if (False(Searching)) get_time_limit(input_string);
    } else if (!memcmp(input_string,"setoption",9)) {
		ptr = strtok(input_string," ");
		for (ptr = strtok(NULL," "); ptr != NULL; ptr = strtok(NULL," ")) {
			if (!memcmp(ptr,"Hash",4) && False(Searching)) {
				ptr += 11;
				value = atoi(ptr);
				init_hash(value);
			} else if (!memcmp(ptr,"MultiPV",7)) {
				ptr += 14;
			    PVN = atoi(ptr);
				Stop = 1;
			} else if (!memcmp(ptr,"Ponder",6)) {
				ptr += 13;
				if (ptr[0] == 't') Ponder = 1;
				else Ponder = 0;
			} else if (!memcmp(ptr,"Clear",5)) {
				memset(Hash,0,hashSize * sizeof(GEntry));
				memset(PVHash,0,pv_hash_size * sizeof(GPVEntry));
			}
        }
	} else if (!strcmp(input_string,"stop")) {
		Stop = 1;
		if (False(Searching)) send_best_move();
	} else if (!strcmp(input_string,"ponderhit")) {
		Infinite = 0;
		if (LastTime > TimeLimit1) Stop = 1;
		if (False(Change) && LastTime * 16 > TimeLimit1 * NormalMargin) Stop = 1;
		if (True(Early) && LastTime * 16 > TimeLimit1 * EarlyMargin) Stop = 1;
		if (True(Easy) && LastTime * 16 > TimeLimit1 * EasyMargin) Stop = 1;
		if (False(Searching)) send_best_move();
	}
    else if (!strcmp(input_string,"quit")) {
		free(Hash);
		exit(0);
	}
	else if (!memcmp(input_string,"bench",5)) bench(input_string);
}

void main() {
	cout << NAME << " " << VERSION << endl;
	DWORD p;
	int CPUInfo[4] = {-1};
    __cpuid(CPUInfo, 0x00000001);
    HardwarePopCnt = (CPUInfo[2] >> 23) & 1;

	StreamHandle = GetStdHandle(STD_INPUT_HANDLE);
	Console = GetConsoleMode(StreamHandle,&p);
	if (Console) {
		SetConsoleMode(StreamHandle,p & (~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT)));
		FlushConsoleInputBuffer(StreamHandle);
	}

	date = 1;
	Ponder = 0;
	init_hash(32);
	memset(Data,0,128 * sizeof(GData));
	init();

	init_keys();
	init_material();
	init_search();

	setbuf(stdout, NULL);
    setbuf(stdin, NULL);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
    fflush(NULL);
	while (1) uci();
}