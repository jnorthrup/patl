/*-
 | Burrows-Wheeler sorting program, v3.10 [O(n)-encoding with suffix tries]
 |
 | (c) 2004-2005 by Roman S. Klyujkov, rsk_comp at mail.ru
 | This is free software under GPL, http://www.gnu.org/licenses/gpl.txt
-*/

#include <uxn/patl/aux_/buffered_io.hpp>
#include <uxn/patl/aux_/perf_timer.hpp>
#include <uxn/patl/suffix_set.hpp>

namespace patl = uxn::patl;

typedef patl::suffix_set<unsigned char*> SuffixSet;

// L - lenght of sorting buffer
// dbuff - buffer of lenght 2*L+1, source data in first half
// in result we have sorted data in first half of dbuff
// return value - position of zero symbol
static unsigned bwtSort(unsigned L, unsigned char *dbuff, SuffixSet &sufSet, unsigned &mult)
{
    // copy 1st half into 2nd
    memcpy(dbuff + L, dbuff, L);
    // stopbyte
    dbuff[2 * L] = ~dbuff[0];
    //
    for (;;)
    {
        // add next node and check for loop
        const SuffixSet::vertex v = sufSet.push_back();
        const unsigned skip = v.skip();
        if (sufSet.size() - 1 + skip / 8 == 2 * L)
        {
            sufSet.pop_back();
            mult = L / sufSet.size();
            break;
        }
    }
    //
    SuffixSet::const_iterator cit = sufSet.begin();
    unsigned id = 0, offs;
    for (; (offs = *cit - dbuff) != 0; ++cit)
        dbuff[id++] = dbuff[offs + L - 1];
    const unsigned zeroId = id;
    for (; cit != sufSet.end(); ++cit)
        dbuff[id++] = dbuff[(*cit - dbuff) + L - 1];
    return zeroId;
}

// L - lenght of unsorting buffer
// dbuff - buffer of lenght 2*L, source data in first half
// pos - position of zero symbol (propagate zeroId)
// vect - allocated memory for storing backward transform vector, size (0x100+L)
// we have resulting data in second half of dbuff
static void bwtUnsort(unsigned L, unsigned char *dbuff, unsigned pos, unsigned *vect)
{
    unsigned *freq = vect;
    vect += 0x100;
    std::fill(freq, freq + 0x100, 0);
    unsigned id;
    for (id = 0; id != L; ++id)
        ++freq[dbuff[id]];
    unsigned sum = 0;
    for (id = 0; id != 0x100; ++id)
        freq[id] = (sum += freq[id]) - freq[id];
    for (id = 0; id != L; ++id)
        vect[freq[dbuff[id]]++] = id;
    for (id = 0; id != L; ++id)
        dbuff[L + id] = dbuff[pos = vect[pos]];
}

static inline unsigned readSize(const std::string &str)
{
    unsigned num = 0;
    std::string::const_iterator it = str.begin();
    for (; it != str.end() && '0' <= *it && *it <= '9'; ++it)
    {
        num *= 10;
        num += *it - '0';
    }
    if (it != str.end())
    {
        switch (*it)
        {
        case 'm':
            num <<= 10;
        case 'k':
            num <<= 10;
            break;
        }
    }
    return num;
}

int main(int argc, char *argv[])
{
    const char copyRight[] =
        "Burrows-Wheeler sorting program, v3.10. Written by Roman S. Klyujkov\n";
    const char helpStr[] =
        "USORT <e|d> infile outfile [blockSize]\n"
        "\tblockSize (e command only) like 1m == 1024k (default 2m)\n";
    int opId = -1;
    if (argc >= 4 && argv[1][1] == 0)
        opId =
        argv[1][0] == 'e' || argv[1][0] == 'E' ? 0 :
        argv[1][0] == 'd' || argv[1][0] == 'D' ? 1 :
        argv[1][0] == 't' || argv[1][0] == 'T' ? 2 : -1;
    printf("%s\n", copyRight);
    try
    {
        if (opId != -1)
        {
            patl::aux::buffered_input bufIn(argv[2]);
            patl::aux::buffered_output bufOut(argv[3]);
            unsigned blockSize = 2 << 20;
            unsigned long realSize;
            patl::aux::performance_timer tim;
            double secs = 0.;
            if (opId == 0) // encode
            {
                // sort
                if (argv[4])
                    blockSize = readSize(argv[4]);
                printf("Sort using %d block size... ", blockSize);
                //
                unsigned char *dbuff = static_cast<unsigned char*>(
                    ::VirtualAlloc(
                        0,
                        2 * blockSize + 1,
                        MEM_COMMIT,
                        PAGE_READWRITE));
                SuffixSet sufSet(dbuff, blockSize + 1);
                do
                {
                    realSize = bufIn.read_bytes(dbuff, blockSize);
                    if (realSize)
                    {
                        unsigned mult;
                        tim.start();
                        const unsigned zeroId =
                            bwtSort(realSize, dbuff, sufSet, mult);
                        tim.finish();
                        secs += tim.get_seconds();
                        sufSet.clear();
                        bufOut.write_word(
                            realSize | (mult != 1 ? 0x80000000 : 0));
                        if (mult != 1)
                            bufOut.write_word(mult);
                        bufOut.write_word(zeroId);
                        bufOut.write_bytes(dbuff, realSize / mult);
                    }
                } while (realSize == blockSize);
                ::VirtualFree(dbuff, 0, MEM_RELEASE);
            }
            else if (opId == 1) // decode
            {
                // unsort
                blockSize = 0;
                printf("Unsort... ");
                //
                unsigned char *dbuff = 0;
                unsigned *vect = 0;
                for (; bufIn.is_data(); )
                {
                    const unsigned rsRead = bufIn.read_dword();
                    realSize = rsRead & 0x7FFFFFFF;
                    unsigned mult = 1;
                    if (realSize != rsRead)
                        realSize /= mult = bufIn.read_dword();
                    if (realSize > blockSize)
                    {
                        if (blockSize)
                        {
                            ::VirtualFree(vect, 0, MEM_RELEASE);
                            ::VirtualFree(dbuff, 0, MEM_RELEASE);
                        }
                        dbuff = static_cast<unsigned char*>(
                            ::VirtualAlloc(
                                0,
                                2 * realSize,
                                MEM_COMMIT,
                                PAGE_READWRITE));
                        vect = static_cast<unsigned*>(
                            ::VirtualAlloc(
                                0,
                                (0x100 + realSize) * sizeof(unsigned),
                                MEM_COMMIT,
                                PAGE_READWRITE));
                        blockSize = realSize;
                    }
                    const unsigned zeroId = bufIn.read_dword();
                    bufIn.read_bytes(dbuff, realSize);
                    tim.start();
                    bwtUnsort(realSize, dbuff, zeroId, vect);
                    tim.finish();
                    secs += tim.get_seconds();
                    for (unsigned id = 0; id != mult; ++id)
                        bufOut.write_bytes(dbuff + realSize, realSize);
                }
                if (blockSize)
                {
                    ::VirtualFree(vect, 0, MEM_RELEASE);
                    ::VirtualFree(dbuff, 0, MEM_RELEASE);
                }
            }
            printf("%.2f secs.\n", secs);
        }
        else
            printf("%s", helpStr);
    }
    catch (const std::exception &exc)
    {
        printf("Exception: %s\n", exc.what());
    }
}
