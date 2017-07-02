#pragma once

namespace mmh
{
    using permutation_base_t = pfc::list_t<t_size>;

    class permutation_t : public permutation_base_t
    {
    public:
        void reset()
        {
            t_size j, count=get_size();
            t_size * ptr = get_ptr();
            for (j=0; j<count; j++)
                ptr[j] = j;
        }
        void reset_reverse()
        {
            t_size j, count=get_size();
            t_size * ptr = get_ptr();
            for (j=0; j<count; j++)
                ptr[j] = count-j-1;
        }
        void set_size(t_size size)
        {
            pfc::list_t<t_size>::set_size(size);
            reset();
        }
        void set_count(t_size size)
        {
            set_size(size);
        };
        permutation_t(t_size size)
        {
            set_size(size);
            reset();
        };
        permutation_t() {};

        using pfc::list_t<t_size>::operator[];
    };

    class permutation_inverse_t : public permutation_t
    {
    public:
        permutation_inverse_t(const permutation_t & p_source)
        {
            t_size i, count = p_source.get_count();
            permutation_base_t::set_size (count);
            for(i=0;i<count;i++)
                (*this)[p_source[i]] = i;
        }
    };

    class permutation_reverse_t : public permutation_t
    {
    public:
        permutation_reverse_t(const permutation_t & p_source)
        {
            t_size i, count = p_source.get_count();
            permutation_base_t::set_size (count);
            for(i=0;i<count;i++)
                (*this)[i] = p_source[count-i-1];
        }
    };

    template <typename List, typename Comparator>
    class ComparatorWrapper
    {
        Comparator m_compare;
        List& m_list;
        bool m_reverse;
    public:
        bool operator()(const t_size& item1, const t_size& item2) const
        {
            int diff = m_compare(m_list[item1], m_list[item2]);
            return m_reverse ? diff>0 : diff<0;
        }
        ComparatorWrapper(List& p_list, Comparator& p_compare, bool b_reverse)
            : m_compare(p_compare), m_list(p_list), m_reverse(b_reverse) {}
    };

    template <typename List, typename Comparator>
    void sort_get_permuation(List&& p_items, permutation_t& p_out, Comparator&& p_compare, bool stabilise, bool b_reverse = false)
    {
        t_size psize = pfc::array_size_t(p_out);
        t_size* out_ptr = p_out.get_ptr();
        ComparatorWrapper<List, Comparator> p_context(p_items, p_compare, b_reverse);
        if (stabilise)
            std::stable_sort(out_ptr, out_ptr + psize, p_context);
        else
            std::sort(out_ptr, out_ptr + psize, p_context);
    }

    template <typename t_item, template<typename> class t_alloc, typename t_compare>
    void remove_duplicates(pfc::list_t<t_item, t_alloc>& p_handles, t_compare p_compare)
    {
        t_size count = p_handles.get_count();
        if (count>0)
        {
            t_item * p_list = p_handles.get_ptr();
            bit_array_bittable mask(count);
            permutation_t order(count);

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
    class bsearch_callback_impl_simple_partial_t : public pfc::bsearch_callback {
    public:
        int test(t_size p_index) const override {
            return m_compare(m_container[m_base+p_index],m_param) * (m_is_reversed ? -1 : 1);
        }
        bsearch_callback_impl_simple_partial_t(const t_container & p_container,t_compare p_compare,const t_param & p_param, t_size base, bool is_reversed = false)
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
    bool bsearch_partial_t(t_size p_count,const t_container & p_container,t_compare p_compare,const t_param & p_param,t_size base,t_size & p_index, bool is_reversed = false) 
    {
        t_size index = p_index;
        bool ret = bsearch(
            p_count,
            bsearch_callback_impl_simple_partial_t<t_container,t_compare,t_param>(p_container,p_compare,p_param,base, is_reversed),
            index);
        p_index = index + base;
        return ret;
    }

#if 0
    template<typename t_container,typename t_compare, typename t_param>
    bool bsearch_nearest(t_size p_count,const t_container & p_container,t_compare p_compare,const t_param & p_param,t_size & p_index) {
        return bsearch(
            p_count,
            bsearch_callback_impl_simple_t<t_container,t_compare,t_param>(p_container,p_compare,p_param),
            p_index);
    }

    template<typename t_callback>
    void __bsearch_nearest(t_size p_count, const t_callback & p_callback,t_size & p_result)
    {
        t_size max = m_items.get_count();
        t_size min = 0;
        t_size ptr;
        while(min<max)
        {
            ptr = min + ( (max-min) >> 1);
            int result = p_callback.test(ptr);
            if (result<0) min = ptr + 1;
            else if (result>0) max = ptr;
            else 
            {
                return ptr;
                //return true;
            }
        }
        if (min==m_items.get_count()) min--;
        return min;
        //return true;
    }
#endif

}
