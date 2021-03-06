/*##############################################################################

    HPCC SYSTEMS software Copyright (C) 2012 HPCC Systems®.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
############################################################################## */

#ifndef RTLKEY_INCL
#define RTLKEY_INCL
#include "eclrtl.hpp"

enum KeySegmentMonitorSerializeType
{
    KSMST_none,                  // can't serialize
    KSMST_WILDKEYSEGMENTMONITOR,
    KSMST_SETKEYSEGMENTMONITOR,
    KSMST_SINGLEKEYSEGMENTMONITOR,
    KSMST_SINGLEBIGSIGNEDKEYSEGMENTMONITOR,
    KSMST_SINGLELITTLESIGNEDKEYSEGMENTMONITOR,
    KSMST_CSINGLELITTLEKEYSEGMENTMONITOR,
    KSMST_OVERRIDEABLEKEYSEGMENTMONITOR,
    KSMST_max
};

interface ITransition : extends IInterface
{
    virtual bool getState() const = 0;
    virtual const void *getValue() const = 0;
    virtual MemoryBuffer &serialize(size32_t size, MemoryBuffer &buffer) const = 0;
};

interface IStringSet : public IInterface
{
    virtual void addRange(const void *loval, const void *hival) = 0;
    virtual void addAll() = 0;
    virtual ITransition *queryTransition(unsigned idx) = 0;
    virtual bool getTransitionValue(void *value, unsigned idx) = 0;
    virtual void killRange(const void *loval, const void *hival) = 0;
    virtual bool inRange(const void *val) const = 0;
    virtual bool inRange(const void *val, unsigned &transition) const = 0;
    virtual size32_t getSize() = 0;
    virtual void reset() = 0;
    virtual unsigned transitionCount() = 0;
    virtual const char *describe(StringBuffer &ret) = 0;
    virtual IStringSet *invertSet() = 0;
    virtual IStringSet *unionSet(IStringSet *) = 0;
    virtual IStringSet *intersectSet(IStringSet *) = 0;
    virtual bool isEmptySet() const = 0;
    virtual bool isFullSet() const = 0;
    virtual bool isSingleValue() const = 0;
    virtual bool isSigned() const = 0;
    virtual bool isBigEndian() const = 0;
    virtual unsigned numValues() const = 0;

    virtual int memcmp(const void *val1, const void *val2, size32_t size) const = 0; 
    virtual bool decrement(void *val) const = 0;
    virtual bool increment(void *val) const = 0;
    virtual MemoryBuffer &serialize(MemoryBuffer &buffer) const = 0;
};

ECLRTL_API IStringSet *createStringSet(size32_t size);
ECLRTL_API IStringSet *createStringSet(size32_t size, bool bigEndian, bool isSigned);
ECLRTL_API IStringSet *deserializeStringSet(MemoryBuffer &mb);

ECLRTL_API int memcmpbigsigned(const void *l, const void *r, unsigned size);
ECLRTL_API int memcmplittleunsigned(const void *l, const void *r, unsigned size);
ECLRTL_API int memcmplittlesigned(const void *l, const void *r, unsigned size);

class RtlRow;

interface IKeySegmentMonitor : public IInterface
{
public:
    virtual bool increment(void * expandedRow) const = 0;
    virtual void setLow(void * expandedRow) const = 0;
    virtual void setHigh(void * expandedRow) const = 0;
    virtual void endRange(void * expandedRow) const = 0;

    virtual bool matchesBuffer(const void * expandedRow) const = 0;
    virtual bool matches(const RtlRow * rawRow) const = 0;

    virtual bool isWild() const = 0;
    virtual unsigned getFieldIdx() const = 0;
    virtual unsigned getOffset() const = 0;
    virtual unsigned getSize() const = 0;
    virtual bool isEmpty() const = 0;
    virtual bool equivalentTo(const IKeySegmentMonitor &other) const = 0;
    virtual bool isSigned() const = 0;
    virtual bool isLittleEndian() const = 0;

    virtual int docompare(const void * expandedLeft, const void * rawRight) const = 0;
    virtual unsigned queryHashCode() const = 0;
    virtual bool isWellKeyed() const = 0;
    virtual bool isOptional() const = 0;

    virtual bool isSimple() const = 0;
    virtual void copy(void *expandedRow, const void *rawRow) const = 0;
    virtual MemoryBuffer &serialize(MemoryBuffer &mb) const = 0;
    virtual KeySegmentMonitorSerializeType serializeType() const = 0;
    virtual IKeySegmentMonitor *clone() const = 0;
    virtual unsigned numFieldsRequired() const = 0;

    virtual bool setOffset(unsigned _offset) = 0;  // Used by old record layout translator - to be removed at some point
};

interface IOverrideableKeySegmentMonitor  : public IKeySegmentMonitor
{
    virtual void setOverrideBuffer(const void *ptr) = 0;
};

interface IBlobProvider
{
    virtual byte * lookupBlob(unsigned __int64 id) = 0;         // return reference, not freed by code generator, can dispose once transform() has returned.
};

interface IBlobCreator
{
    virtual unsigned __int64 createBlob(size32_t _size, const void * _ptr) = 0;
};

enum FFoption : byte
{
    FFkeyed = 1,
    FFopt = 1,
    FFpost = 2,
};

interface IFieldFilter;
interface IIndexReadContext
{
public:
    virtual void append(IKeySegmentMonitor *segment) = 0;
    virtual unsigned ordinality() const = 0;
    virtual IKeySegmentMonitor *item(unsigned idx) const = 0;
    virtual void append(FFoption option, IFieldFilter * filter) = 0;
};

ECLRTL_API IStringSet *createRtlStringSet(size32_t size);
ECLRTL_API IStringSet *createRtlStringSetEx(size32_t size, bool bigEndian, bool isSigned);
ECLRTL_API IStringSet *createRtlStringValue(size32_t size, const char * value);

//Following can optionally return either of their arguments as the result.
ECLRTL_API IStringSet *rtlUnionSet(IStringSet * lhs, IStringSet * rhs);
ECLRTL_API IStringSet *rtlIntersectSet(IStringSet * lhs, IStringSet * rhs);

// Various types of KeySegmentMonitor, called from generated code

ECLRTL_API IKeySegmentMonitor *createKeySegmentMonitor(bool optional, IStringSet *set, unsigned _fieldIdx, unsigned _offset, unsigned _size);
ECLRTL_API IKeySegmentMonitor *createEmptyKeySegmentMonitor(bool optional, unsigned _fieldIdx, unsigned _offset, unsigned _size);
ECLRTL_API IKeySegmentMonitor *createWildKeySegmentMonitor(unsigned _fieldIdx, unsigned _offset, unsigned _size);
ECLRTL_API IKeySegmentMonitor *createDummyKeySegmentMonitor(unsigned _fieldIdx, unsigned _offset, unsigned _size, bool isSigned, bool isLittleEndian);
ECLRTL_API IKeySegmentMonitor *createSingleKeySegmentMonitor(bool optional, unsigned _fieldIdx, unsigned _offset, unsigned _size, const void * value);
ECLRTL_API IKeySegmentMonitor *createSingleBigSignedKeySegmentMonitor(bool optional, unsigned _fieldIdx, unsigned offset, unsigned size, const void * value);
ECLRTL_API IKeySegmentMonitor *createSingleLittleSignedKeySegmentMonitor(bool optional, unsigned _fieldIdx, unsigned offset, unsigned size, const void * value);
ECLRTL_API IKeySegmentMonitor *createSingleLittleKeySegmentMonitor(bool optional, unsigned _fieldIdx, unsigned offset, unsigned size, const void * value);

ECLRTL_API IOverrideableKeySegmentMonitor *createOverrideableKeySegmentMonitor(IKeySegmentMonitor *base);

ECLRTL_API IKeySegmentMonitor *deserializeKeySegmentMonitor(MemoryBuffer &mb);
ECLRTL_API void deserializeSet(IStringSet & set, size32_t minRecordSize, const RtlTypeInfo * fieldType, const char * filter);

//---------------------------------------------------------------------------------------------------------------------

enum TransitionMask : byte
{
    CMPlt = 0x01,
    CMPeq = 0x02,
    CMPgt = 0x04,
    CMPmin = 0x08,      //minimum possible value
    CMPmax = 0x10,      //maximum possible value
    CMPle = CMPlt | CMPeq,
    CMPge = CMPgt | CMPeq,
    CMPminmask = CMPgt|CMPmin,
    CMPmaxmask = CMPlt|CMPmax,
};

class ValueTransition;
interface RtlTypeInfo;
class RtlRow;

/*
 * The IValueSet interface represents a set of ranges of values.
 *
 * The transitions always come in pairs - an upper and lower bound.  Each bound can be inclusive or exclusive.
 */
interface IValueSet : public IInterface
{
//The following methods are used for creating a valueset
    virtual ValueTransition * createTransition(TransitionMask mask, unsigned __int64 value) const = 0;
    virtual ValueTransition * createStringTransition(TransitionMask mask, size32_t len, const char * value) const = 0;
    virtual ValueTransition * createUtf8Transition(TransitionMask mask, size32_t len, const char * value) const = 0;
    virtual void addRange(ValueTransition * loval, ValueTransition * hival) = 0;
    virtual void addAll() = 0;
    virtual void killRange(ValueTransition * loval, ValueTransition * hival) = 0;
    virtual void reset() = 0;
    virtual void invertSet() = 0;
    virtual void unionSet(const IValueSet *) = 0;
    virtual void excludeSet(const IValueSet *) = 0;
    virtual void intersectSet(const IValueSet *) = 0;
    virtual StringBuffer & serialize(StringBuffer & out) const= 0;

//following are primarily for use from the code generator
    virtual ValueTransition * createRawTransition(TransitionMask mask, const void * value) const = 0;
    virtual void addRawRange(const void * lower, const void * upper) = 0;
    virtual void killRawRange(const void * lower, const void * upper) = 0;

//The following methods are for use once the value set has been created.
    virtual unsigned numRanges() const = 0;

    //find the last range where the lower bound <= the field, returns 0 if the field matches the lower bound, > 0 otherwise.
    //matchRange is set to the range number, set to numRanges if there is no possible match.  Uses a binary chop
    virtual int findCandidateRange(const byte * field, unsigned & matchRange) const = 0;

    //find the last range where the lower bound <= the field, returns 0 if the field matches the lower bound, > 0 otherwise.
    //starts searching from curRange (which is likely to match).  Uses a sequential search.
    virtual int checkCandidateRange(const byte * field, unsigned & curRange) const = 0;

    // Does this field match any range?
    virtual bool matches(const byte * field) const = 0;

    // Does this field match this particular range?
    virtual bool matches(const byte * field, unsigned range) const = 0;

    virtual const RtlTypeInfo & queryType() const = 0;
};
extern ECLRTL_API IValueSet * createValueSet(const RtlTypeInfo & type);

interface ISetCreator
{
public:
    virtual void addRange(TransitionMask lowerMask, const StringBuffer & lower, TransitionMask upperMask, const StringBuffer & upperString) = 0;
};

/*
 * Read the textual representation of a set.
 * The format of the set is an optional comma-separated sequence of ranges.
 * Each range is specified as paren lower, upper paren, where the paren is either ( or [ depending
 * on whether the specified bound is inclusive or exclusive.
 * If only one bound is specified then it is used for both upper and lower bound (only meaningful with [] )
 *
 * ( A means values > A - exclusive
 * [ means values >= A - inclusive
 * A ) means values < A - exclusive
 * A ] means values <= A - inclusive
 * For example:
 * [A] matches just A
 * (,A),(A,) matches all but A
 * (A] of [A) are both empty ranges
 * [A,B) means A*
 * Values use the ECL syntax for constants. String constants are always utf8. Binary use d'xx' format (hexpairs)
 *
 * @param creator   The interface that wraps the set that is being created
 * @param filter    The textual representation of the set.
 */
extern ECLRTL_API void deserializeSet(ISetCreator & creator, const char * filter);

/*
 * Read the textual representation of a set.
 *
 * @param set       The target set to be updated.
 * @param filter    The textual representation of the set.
 */
extern ECLRTL_API void deserializeSet(IValueSet & set, const char * filter);


/*
 * This interface represents a filter on a particular field of the row - similar to a segment monitor.
 *
 * Example implementations include single value, sets of ranges, regex or wildcard
 */
interface IFieldFilter : public IInterface
{
public:
//Simple row matching
    virtual bool matches(const RtlRow & row) const = 0;

    virtual int compareRow(const RtlRow & left, const RtlRow & right) const = 0;

    //MORE to come to support index lookups.
    virtual unsigned numRanges() const = 0;
    virtual int findCandidateRange(const RtlRow & row, unsigned & matchRange) const = 0;
    virtual int checkCandidateRange(const RtlRow & row, unsigned & curRange) const = 0;
    virtual bool withinUpperRange(const RtlRow & row, unsigned range) const = 0; // is the row within the current upper limit?
    virtual bool matches(const RtlRow & row, unsigned range) const = 0;
};

//More types of IFieldFilter to come later
extern ECLRTL_API IFieldFilter * createEmptyFieldFilter(unsigned _fieldId, const RtlTypeInfo & type);
extern ECLRTL_API IFieldFilter * createFieldFilter(unsigned fieldId, const RtlTypeInfo & type, const void * value);
extern ECLRTL_API IFieldFilter * createFieldFilter(unsigned fieldId, IValueSet * values);
extern ECLRTL_API IFieldFilter * createWildFieldFilter(unsigned fieldId);



#endif
