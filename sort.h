#pragma once

namespace mmh
{
    using PermutationBase = pfc::list_t<t_size>;

    class Permuation : public PermutationBase
    {
    public:
        void reset()
        {
            const t_size count=get_size();
            t_size * ptr = get_ptr();
            for (t_size j = 0; j<count; j++)
                ptr[j] = j;
        }
        void reset_reverse()
        {
            const t_size count=get_size();
            t_size * ptr = get_ptr();
            for (t_size j = 0; j<count; j++)
                ptr[j] = count-j-1;
        }
        void set_size(t_size size)
        {
            PermutationBase::set_size(size);
            reset();
        }
        void set_count(t_size size)
        {
            set_size(size);
        }
        Permuation(t_size size)
        {
            set_size(size);
            reset();
        }
        Permuation() {}

        using PermutationBase::operator[];
    };

    class InversePermutation : public Permuation
    {
    public:
        InversePermutation(const Permuation & p_source)
        {
            const t_size count = p_source.get_count();
            PermutationBase::set_size (count);
            for(t_size i = 0;i<count;i++)
                (*this)[p_source[i]] = i;
        }
    };

    class ReversePermutation : public Permuation
    {
    public:
        ReversePermutation(const Permuation & p_source)
        {
            const t_size count = p_source.get_count();
            PermutationBase::set_size (count);
            for(t_size i = 0;i<count;i++)
                (*this)[i] = p_source[count-i-1];
        }
    };

    template<typename List, typename Comparator>
    class ComparatorWrapper
    {
        Comparator& m_compare;
        List& m_list;
        bool m_reverse;
        bool m_stabilise;
    public:
        bool operator()(const t_size& item1, const t_size& item2) const
        {
            int diff = m_compare(m_list[item1], m_list[item2]);
            if (m_stabilise && !diff)
                return item1 < item2;
            return m_reverse ? diff>0 : diff<0;
        }
        // Note: m_compare{p_compare} does not compile under VS2015, but compiles under VC2017 and Clang
        ComparatorWrapper(List& p_list, Comparator& p_compare, bool b_reverse, bool stabilise = false)
            : m_compare(p_compare), m_list{p_list}, m_reverse{b_reverse}, m_stabilise{stabilise}  {}
    };

    template<typename List, typename Comparator>
    void sort_get_permuation(List&& p_items, Permuation& p_out, Comparator&& p_compare, bool stabilise, bool b_reverse = false, 
                             bool allow_parallelisation = false, size_t parallel_chunk_size = 512)
    {
        t_size psize = pfc::array_size_t(p_out);
        t_size* out_ptr = p_out.get_ptr();
        if (allow_parallelisation && psize >= parallel_chunk_size) {
            // C++17 has parallel sort, but it is not implemented in VC2017 yet.
            ComparatorWrapper<List, Comparator> p_context(p_items, p_compare, b_reverse, stabilise);
            concurrency::parallel_buffered_sort(out_ptr, out_ptr + psize, p_context, parallel_chunk_size);
        } 
        else {
            ComparatorWrapper<List, Comparator> p_context(p_items, p_compare, b_reverse);
            if (stabilise)
                std::stable_sort(out_ptr, out_ptr + psize, p_context);
            else
                std::sort(out_ptr, out_ptr + psize, p_context);
        }
    }

    template<typename List>
    void destructive_reorder(List& items, mmh::Permuation& perm)
    {
        for (size_t i = 1; i < perm.get_size(); ++i) {
            t_size current = i;
            t_size next = perm[i];
            while (next != i) {
                std::swap(items[current], items[next]);
                perm[current] = current;
                current = next;
                next = perm[next];
            }
            perm[current] = current;
        }
    }


    template<typename t_item, template<typename> class t_alloc, typename t_compare>
    void remove_duplicates(pfc::list_t<t_item, t_alloc>& p_handles, t_compare p_compare)
    {
        t_size count = p_handles.get_count();
        if (count>0)
        {
            t_item * p_list = p_handles.get_ptr();
            bit_array_bittable mask(count);
            Permuation order(count);

            sort_get_permuation(p_list, order, p_compare, false, false);
            
            t_size n;
            bool found = false;
            for(n=0;n<count-1;n++)
            {
                if (p_list[order[n]]==p_list[order[n+1]])
                {
                    found = true;
                    mask.set(order[n+1],true);
                }
            }
            
            if (found) p_handles.remove_mask(mask);
        }
    }

    template<typename t_container,typename t_compare, typename t_param>
    class PartialBSearchCallback : public pfc::bsearch_callback {
    public:
        int test(t_size p_index) const override {
            return m_compare(m_container[m_base+p_index],m_param) * (m_is_reversed ? -1 : 1);
        }
        PartialBSearchCallback(const t_container & p_container,t_compare p_compare,const t_param & p_param, t_size base, bool is_reversed = false)
            : m_container(p_container), m_compare(p_compare), m_param(p_param), m_base(base), m_is_reversed(is_reversed)
        {
        }
    private:
        const t_container & m_container;
        t_compare m_compare;
        const t_param & m_param;
        t_size m_base;
        bool m_is_reversed;
    };

    template<typename t_container,typename t_compare, typename t_param>
    bool partial_bsearch(t_size p_count,const t_container & p_container,t_compare p_compare,const t_param & p_param,t_size base,t_size & p_index, bool is_reversed = false) 
    {
        t_size index = p_index;
        bool ret = bsearch(
            p_count,
            PartialBSearchCallback<t_container,t_compare,t_param>(p_container,p_compare,p_param,base, is_reversed),
            index);
        p_index = index + base;
        return ret;
    }
}
