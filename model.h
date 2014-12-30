#ifndef MODEL_H
#define MODEL_H

#include "base.h"
#include "data_io.h"

#include <vector>
#include <set>
#include <memory>

namespace db_compress {

/*
 * Structure used to represent any probability interval between [0, 1].
 */
struct ProbInterval {
    double l, r;
};

/*
 * ProbDist Class is initialized with certain ProbInterval and some Probability
 * Distribution, it reads in bit string, reaches certain unit bin in the distribution
 * and emits the result bin, the result ProbInterval, and the remain unused bits.
 */
class ProbDist {
  public:
    virtual ~ProbDist() = 0;
    virtual bool End() = 0;
    // Will only use the last 8 bits.
    virtual void FeedByte(int byte) = 0;
    // The most significant bit is only used to indicate the end. Thus, 11001 represents
    // four unused bits 1001.
    virtual int GetUnusedBits() = 0;
    virtual const ProbInterval& GetRemainProbInterval() = 0;
    virtual AttrValue* GetResult() = 0;
    // Reset the ProbDist to its initial state, i.e., no bits has been read in.
    virtual void Reset() = 0;
};

inline ProbDist::~ProbDist() {}

/*
 * The Model class represents the local conditional probability distribution. The
 * Model object can be used to generate ProbDist object which can be used to infer
 * the result attribute value based on bitstring (decompressing). It can also be used to
 * create ProbInterval object which can be used for compressing.
 */
class Model {
  public:
    // The Model class owns the ProbDist object.
    virtual ~Model() = 0;
    virtual ProbDist* GetProbDist(const Tuple& tuple, const ProbInterval& prob_interval) = 0;
    virtual ProbInterval GetProbInterval(const Tuple& tuple, const ProbInterval& prob_interval, std::vector<char>* emit_bytes) = 0;
    virtual const std::vector<int>& GetPredictorList() = 0;
    virtual int GetTargetVar() = 0;

    // Learning
    virtual void FeedTuple(const Tuple& tuple) { }
    virtual void EndOfData() { }
};

inline Model::~Model() {}

/*
 * The ModelLearner class learns all the models simultaneously in an online fashion.
 */
class ModelLearner {
  private:
    Schema schema_;
    CompressionConfig config_;
    int stage_;
    std::vector<int> ordered_attr_list_;
    std::vector< std::unique_ptr<Model> > active_model_list_;
    std::vector< std::unique_ptr<Model> > model_list_;
    std::vector< std::unique_ptr<Model> > selected_model_;
    std::vector< std::vector<int> > model_predictor_list_;
    
    void InitModelList();
    void ExpandModelList();
  public:
    ModelLearner(const Schema& schema, const CompressionConfig& config);
    // These functions are used to learn the Model objects.
    void FeedTuple(const Tuple& tuple);
    bool RequireMoreIterations() const;
    void EndOfData();
    // This function gets the Model object for any particular attribute. Caller takes
    // ownership of the Model object. 
    Model* GetModel(int attr_index);
    // This function gets the order of attributes during the encoding/decoding phase
    const std::vector<int>& GetOrderOfAttributes() const;
};

} // namespace db_compress

#endif
