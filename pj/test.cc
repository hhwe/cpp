namespace SHFTest
{
    template <
        class PLA,
        class PLB>
    class PLClass
    {
        //
        // 一般实现
        //
    public:
        PLClass(){};
        ~PLClass(){};
        void FuncA()
        {
        }
    };

    typedef myPLA;
    typedef myPLB;

    //
    // 单参数特化实现,参数A
    //
    template <class PLA>
    class PLClass<PLA, myPLB>
    {
        //
        // 特化实现
        //
        PLClass(){};
        ~PLClass(){};
        void FuncA()
        {
        }
    };

    //
    // 单参数特化实现,参数B
    //
    template <class PLB>
    class PLClass<myPLA, PLB>
    {
        //
        // 特化实现
        //
        PLClass(){};
        ~PLClass(){};
        void FuncA()
        {
        }
    };

    //
    // 双参数特化实现
    //
    template <>
    class PLClass<myPLA, myPLB>
    {
        //
        // 特化实现
        //
        PLClass(){};
        ~PLClass(){};
        void FuncA()
        {
        }
    };
}