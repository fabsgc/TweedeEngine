#pragma once

#include "TeCorePrerequisites.h"
#include "Serialization/TeSerializable.h"

namespace te
{
    /**	Allows you to specify defines that can control shader compilation. */
    class TE_CORE_EXPORT ShaderDefines
    {
    public:
        /** Adds a new define with a floating point value. */
        void Set(const String& name, float value);

        /** Adds a new define with an integer value. */
        void Set(const String& name, INT32 value);

        /** Adds a new define with an integer value. */
        void Set(const String& name, UINT32 value);

        /** Adds a new define with a string point value. */
        void Set(const String& name, const String& value);

        /**	Returns a list of all defines. */
        UnorderedMap<String, String> GetAll() const { return _defines; }

        /** Removes all defines. */
        void clear() { _defines.clear(); }

    protected:
        UnorderedMap<String, String> _defines;
    };

    /**
     * Contains information about a single variation of a Shader. Each variation can have a separate set of
     * \#defines that control shader compilation.
     */
    class TE_CORE_EXPORT ShaderVariation : public Serializable
    {
    public:
        /** Possible types of a variation parameter. */
        enum ParamType
        {
            Int,
            UInt,
            Float,
            Boolean
        };

        /** Name, type and value of a variation parameter. */
        struct Param
        {
            Param()
                : I(0), Type(Int)
            { }

            Param(const String& name, INT32 val)
                : I(val), Name(name), Type(ParamType::Int)
            { }

            Param(const String& name, UINT32 val)
                : Ui(val), Name(name), Type(ParamType::Int)
            { }

            Param(const String& name, float val)
                : F(val), Name(name), Type(ParamType::Float)
            { }

            Param(const String& name, bool val)
                : I(val ? 1 : 0), Name(name), Type(ParamType::Boolean)
            { }

            union
            {
                INT32 I;
                UINT32 Ui;
                float F;
            };

            String Name;
            ParamType Type;
        };

        ShaderVariation();

        /** Creates a new shader variation with the specified parameters. */
        ShaderVariation(const Vector<Param>& params);

        /**
         * Returns the value of a signed integer parameter with the specified name. Returns 0 if the parameter cannot be
         * found.
         */
        INT32 GetInt(const String& name);

        /**
         * Returns the value of a unsigned integer parameter with the specified name. Returns 0 if the parameter cannot be
         * found.
         */
        UINT32 GetUInt(const String& name);

        /** Returns the value of a float parameter with the specified name. Returns 0 if the parameter cannot be found.  */
        float GetFloat(const String& name);

        /**
         * Returns the value of a boolean parameter with the specified name. Returns false if the parameter cannot be
         * found.
         */
        bool GetBool(const String& name);

        /**
         * Sets the value of the parameter for the provided name. Any previous value for a parameter with the same name
         * will be overwritten.
         */
        void SetInt(const String& name, INT32 value);

        /**
         * Sets the value of the parameter for the provided name. Any previous value for a parameter with the same name
         * will be overwritten.
         */
        void SetUInt(const String& name, UINT32 value);

        /**
         * Sets the value of the parameter for the provided name. Any previous value for a parameter with the same name
         * will be overwritten.
         */
        void SetFloat(const String& name, float value);

        /**
         * Sets the value of the parameter for the provided name. Any previous value for a parameter with the same name
         * will be overwritten.
         */
        void SetBool(const String& name, bool value);

        /** Registers a new parameter that controls the variation. */
        void AddParam(const Param& param) { _params[param.Name] = param; }

        /** Removes a parameter with the specified name. */
        void RemoveParam(const String& paramName) { _params.erase(paramName); }

        /** Checks if the variation has a parameter with the specified name. */
        bool HasParam(const String& paramName) { return _params.find(paramName) != _params.end(); }

        /** Removes all parameters. */
        void ClearParams() {_params.clear(); }

        /** Returns a list of names of all registered parameters. */
        Vector<String> GetParamNames() const;

        /**
         * Checks if this variation matches some other variation.
         *
         * @param[in]		other		Other variation to compare it to.
         * @param[in]		exact		When true both variations need to have the exact number of parameters with identical
         *								contents, equivalent to the equals operator. When false, only the subset of
         *								parameters present in @p other is used for comparison, while any extra parameters
         *								present in this object are ignored.
         */
        bool Matches(const ShaderVariation& other, bool exact = true) const;

        /** Returns all the variation parameters. */
        const UnorderedMap<String, Param>& GetParams() const { return _params; }

        bool operator==(const ShaderVariation& rhs) const;

        /** Empty variation with no parameters. */
        static const ShaderVariation EMPTY;

        /** Converts all the variation parameters in a ShaderDefines object, that may be consumed by the shader compiler. */
        ShaderDefines GetDefines() const;

        /**
         * Returns a unique index of this variation, relative to all other variations registered in ShaderVariations object.
         */
        UINT32 GetIdx() const { return _idx; }

        /** Assigns a unique index to the variation that can later be used for quick lookup. */
        void SetIdx(UINT32 idx) const { _idx = idx; }

    private:
        friend class ShaderVariations;

        UnorderedMap<String, Param> _params;
        mutable UINT32 _idx = static_cast<UINT32>(-1);
    };

    /** A container for all variations of a single Shader. */
    class TE_CORE_EXPORT ShaderVariations
    {
    public:
        /** Registers a new variation. */
        void Add(const ShaderVariation& variation);

        /** Returns a variation at the specified index. Variations are indexed sequentially as they are added. */
        const ShaderVariation& Get(UINT32 idx) { return _variations[idx]; }

        /** Get() returns a reference and don't do any check. This method is usefull to prevent the application from crashing */
        const bool Exist(UINT32 idx) { return _variations.size() > idx; }

        /**
         * Scans a list of stored variations and returns an index of a variation that has the same parameters as the
         * provided one, or -1 if one is not found.
         */
        UINT32 Find(const ShaderVariation& variation) const;

        /** Returns a list of all variations. */
        const Vector<ShaderVariation>& GetVariations() const { return _variations; }

    private:
        Vector<ShaderVariation> _variations;
        UINT32 _nextIdx = 0;
    };
}
