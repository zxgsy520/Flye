#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

#include <bf.h>

#include "fasta.h"


class Kmer
{
public:
	Kmer(const std::string& dnaString);

	Kmer reverseComplement() const;
	Kmer appendRight(char dnaSymbol) const;
	Kmer appendLeft(char dnaSymbol) const;
	std::string dnaRepresentation() const;
	typedef uint64_t KmerRepr;

	bool operator==(const Kmer& other) const
		{return this->_representation == other._representation;}

	struct KmerHash
	{
		std::size_t operator()(const Kmer& kmer) const
		{
			return std::hash<KmerRepr>()(kmer._representation);
		}
	};

private:
	KmerRepr _representation;
};


class VertexIndex
{
public:
	static VertexIndex& getInstance()
	{
		static VertexIndex instance;
		return instance;
	}
	VertexIndex(const VertexIndex&) = delete;
	void operator=(const VertexIndex&) = delete;

	struct ReadPosition
	{
		ReadPosition(FastaRecord::ReadIdType readId, int32_t position):
			readId(readId), position(position) {}
		FastaRecord::ReadIdType readId;
		int32_t position;
	};
	struct KmerPosition
	{
		KmerPosition(Kmer kmer, int32_t position):
			kmer(kmer), position(position) {}
		Kmer kmer;
		int32_t position;
	};

	typedef std::unordered_map<Kmer, std::vector<ReadPosition>, 
					   		   Kmer::KmerHash> KmerIndex;
	typedef std::unordered_map<FastaRecord::ReadIdType, 
					   		   std::vector<KmerPosition>> ReadIndex;

	void 		 setKmerSize(unsigned int size);
	void 		 applyKmerThresholds(unsigned int minCoverage, 
							 		 unsigned int maxCoverage);
	unsigned int getKmerSize() const 
		{return _kmerSize;}
	void 		 addFastaSequence(const FastaRecord& fastaRecord);
	void 		 buildReadIndex();

	const KmerIndex&  getIndexByKmer() const
		{return _kmerIndex;}
	const ReadIndex&  getIndexByRead() const
		{return _readIndex;}

	void outputCounts() const;
private:
	VertexIndex();

	bf::basic_bloom_filter _bloomFilter;

	unsigned int _kmerSize;
	KmerIndex _kmerIndex;
	ReadIndex _readIndex;
};
