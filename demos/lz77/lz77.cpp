#include <uxn/patl/aux_/bitwise_io.hpp>
#include <uxn/patl/suffix_set.hpp>

namespace patl = uxn::patl;

typedef patl::suffix_set<char*> SuffixSet;
typedef SuffixSet::vertex vertex;

template <typename Input, typename Output>
int lzCompress(unsigned nPow, unsigned wPow, unsigned minMatch, Input &src, Output &dst)
{
	int retcod = 0;
	//
	const unsigned
		n = 1 << nPow,
		w = 1 << wPow;
	unsigned count = 0;
	long long total = 0;
	const char stopBytes[] =
		"\x81\xAE\xA4\x11\xB6\x18\x1C\xA7"
		"\xF5\xE8\xF9\xD2\x7C\x2D\x0E\x3C";
	char
		*const buf = new char [2 * n + w + sizeof(stopBytes)],
		*const buf2 = buf + n,
		*const buf3 = buf2 + n;
	memcpy(buf3 + w, stopBytes, sizeof(stopBytes));
	SuffixSet sufs(buf2, n);
	patl::aux::bit_output<Output> bdst(dst);
	bdst.put_fib_code(nPow);
	//put_fib_code(bdst,wPow);
	bdst.put_fib_code(minMatch);
	char *tail;
	for (bool firstTime = true, incomplete = false;
		;
		firstTime = false)
	{
		if (!incomplete)
		{
			char *beg = firstTime ? buf2 : buf2 + w;
			const unsigned reindexCnt = firstTime ? 0 : sufs.count_reindex(beg);
			const bool needReindex = reindexCnt < sufs.size() / 4;
			if (!firstTime && needReindex)
				sufs.pop_reindex(reindexCnt);
			const unsigned
				toread = firstTime ? n + w : n,
				avail = src(beg, toread);
			if (!firstTime)
			{
				if (needReindex)
					sufs.push_reindex(reindexCnt);
				else
				{
					unsigned size = sufs.size();
					sufs.clear();
					sufs.push_reindex(size);
				}
			}
			if (!avail)
				break;
			incomplete = avail < toread;
			tail = beg + std::min(toread, avail);
		}
		bool copyState = false;
		unsigned
			copyCount = 0,
			skip = 0;
		const char
			*cur = buf2,
			*end = std::min(buf3, tail);
		for (;
			cur != end;
			++cur, skip = patl::impl::max0(skip - 1))
		{
			if (!firstTime)
				sufs.pop_front();
			vertex v = sufs.push_back();
			if (!skip)
			{
				skip = std::min(
					w,
					std::min(
						patl::impl::max0(v.skip()) / 8,
						patl::impl::unsigned_cast(end - cur)));
				const char *match = v.sibling().key();
				++count;
				if (skip < minMatch)
				{
					++total;
					if (copyState)
					{
						if (copyCount == 65536)
						{
							bdst.put_fib_code(1);
							bdst.put_fib_code(copyCount);
							bdst.put_bytes(cur - copyCount, copyCount);
							copyCount = 1;
						}
						else
							++copyCount;
					}
					else
					{
						copyCount = 1;
						copyState = true;
					}
					skip = 0;
				}
				else
				{
					total += skip;
					if (copyState)
					{
						bdst.put_fib_code(1);
						bdst.put_fib_code(copyCount);
						bdst.put_bytes(cur - copyCount, copyCount);
						copyState = false;
					}
					bdst.put_fib_code(skip - minMatch + 3);
					bdst.put_bits(cur - match, patl::impl::get_highest_bit_id(sufs.size() - 1) + 1);
				}
			}
		}
		if (copyState)
		{
			bdst.put_fib_code(1);
			bdst.put_fib_code(copyCount);
			bdst.put_bytes(cur - copyCount, copyCount);
		}
		if (incomplete && tail < buf3)
			break;
		memmove(buf, buf2, n + w + sizeof(stopBytes));
		tail -= n;
		sufs.rebind(buf);
	}
	bdst.put_fib_code(2); // EOF
	delete[] buf;
	//
	return retcod;
}

//  опирует данные из буфера в буфер, ид€ в пор€дке возрастани€ адресов
// (это важно, поскольку буфера могут пересекатьс€ и в этом случае нужно
// размножить существующие данные) by Bulat Ziganshin
inline void copymem(char *dst, const char *src, unsigned len)
{
	while (len--)
		*dst++ = *src++;
}

template <typename Input, typename Output>
int lzDecompress(Input &src, Output &dst)
{
	int retcod = 0;
	//
	patl::aux::bit_input<Input> bsrc(src);
	const unsigned
		nPow = bsrc.get_fib_code(),
		n = 1 << nPow,
		minMatch = bsrc.get_fib_code();
	char
		*const buf = new char [2 * n/*+w*/],
		*const buf2 = buf + n,
		*const buf3 = buf2 + n;
	for (bool firstTime = true; ; firstTime = false)
	{
		char
			*cur = buf2,
			*end = buf3;
		for (; cur != end; )
		{
			const unsigned code = bsrc.get_fib_code();
			if (code == 1)
			{
				const unsigned copyCount = bsrc.get_fib_code();
				bsrc.get_bytes(cur, copyCount);
				cur += copyCount;
			}
			else if (code == 2)
			{
				dst(buf2, cur - buf2);
				goto eof;
			}
			else
			{
				const unsigned
					skip = code - 3 + minMatch,
					match = bsrc.get_bits(patl::impl::get_highest_bit_id(firstTime ? cur - buf2 : n - 1) + 1);
				copymem(cur, cur - match, skip);
				cur += skip;
			}
		}
		dst(buf2, cur - buf2);
		memcpy(buf, buf2, n);
	}
eof:
	delete[] buf;
	//
	return retcod;
}

class InputFunctor
{
public:
	InputFunctor(const char *fname)
		: fin_(fopen(fname, "rb"))
	{
	}

	~InputFunctor()
	{
		fclose(fin_);
	}

	bool operator!() const
	{
		return !fin_;
	}

	unsigned operator()(void *buf, unsigned size)
	{
		return fread(buf, 1, size, fin_);
	}

private:
	FILE *fin_;
};

class OutputFunctor
{
public:
	OutputFunctor(const char *fname)
		: fout_(fopen(fname, "wb"))
	{
	}

	~OutputFunctor()
	{
		fclose(fout_);
	}

	bool operator!() const
	{
		return !fout_;
	}

	unsigned operator()(const void *buf, unsigned size)
	{
		return fwrite(buf, 1, size, fout_);
	}

private:
	FILE *fout_;
};

void info()
{
	printf("LZ77 <e|d> infile outfile\n");
}

int main(int argc, char *argv[])
{
	if (argc < 4 || argv[1][1])
	{
		info();
		return 0;
	}
	// open input
	InputFunctor fin(argv[2]);
	if (!fin)
	{
		printf("Can't open %s for read\n", argv[2]);
		return 3;
	}
	// open output
	OutputFunctor fout(argv[3]);
	if (!fout)
	{
		printf("Can't open %s for write\n", argv[3]);
		return 4;
	}
	switch (*argv[1])
	{
	case 'e':
		lzCompress(20, 16, 4, fin, fout);
		break;
	case 'd':
		lzDecompress(fin, fout);
		break;
	default:
		info();
	}
}
