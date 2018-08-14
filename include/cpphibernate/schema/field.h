#pragma once

#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>
#include <cpphibernate/schema/getter.h>
#include <cpphibernate/schema/setter.h>

#include <cpputils/misc/indent.h>
#include <cpputils/misc/type_helper.h>

beg_namespace_cpphibernate_schema
{

    namespace __impl
    {

        /* field_t */

        template<typename T_name, typename T_getter, typename T_setter, typename T_attributes>
        struct field_t
        {
            using name_type         = T_name;
            using getter_type       = T_getter;
            using setter_type       = T_setter;
            using attributes_type   = T_attributes;
            using this_type         = field_t<name_type, getter_type, setter_type, attributes_type>;

            name_type           name;
            getter_type         getter;
            setter_type         setter;
            attributes_type     attributes;

            constexpr field_t(
                T_name&&        p_name,
                T_getter&&      p_getter,
                T_setter&&      p_setter,
                T_attributes&&  p_attributes)
                : name          (std::forward<T_name>      (p_name))
                , getter        (std::forward<T_getter>    (p_getter))
                , setter        (std::forward<T_setter>    (p_setter))
                , attributes    (std::forward<T_attributes>(p_attributes))
                { }

            cpphibernate_copyable(field_t, delete);
            cpphibernate_moveable(field_t, default);

            cpphibernate_equality_comparable();

            inline void print(std::ostream& os) const
            {
                using namespace ::utl;
                using value_type   = typename mp::decay_t<getter_type>::value_type;
                using dataset_type = misc::real_dataset_t<value_type>;
                size_t index = 0;
                os  << indent << '{'
                    << incindent
                        << indent << "\"name\": \"" << name << "\","
                        << indent << "\"value_type\": \"" << utl::type_helper<value_type>::name() << "\""
                        << indent << "\"dataset_type\": \"" << utl::type_helper<dataset_type>::name() << "\""
                        << indent << "\"attributes\": "
                        << indent << '['
                        << incindent;
                        hana::for_each(attributes, [&](auto& attrib){
                            if (index++ > 0) os << ",";
                            os << indent << attrib.name;
                        });
                os      << decindent
                        << indent << ']'
                    << decindent
                    << indent << '}';
            }
        };

        /* field_builder */

        template<typename X, typename = void>
        struct field_builder
        {
            template <typename ...T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for hibernate::schema::make_field(...)!"); }
        };

        template<typename T_name, typename T_getter, typename T_setter, typename T_attributes>
        struct field_builder<mp::list<T_name, T_getter, T_setter, T_attributes>, mp::enable_if_c<
                is_getter<mp::decay_t<T_getter>>::value
            &&  is_setter<mp::decay_t<T_setter>>::value
            &&  is_attributes<mp::decay_t<T_attributes>>::value>>
        {
            using field_type = field_t<T_name, T_getter, T_setter, T_attributes>;

            static constexpr decltype(auto) apply(T_name&& name, T_getter&& getter, T_setter&& setter, T_attributes&& attributes)
                { return field_type(std::forward<T_name>(name), std::forward<T_getter>(getter), std::forward<T_setter>(setter), std::forward<T_attributes>(attributes)); }
        };

    }

    /* meta */

    template<typename T>
    struct is_field
        : public mp::is_specialization_of<T, __impl::field_t>
        { };

    template<typename... T>
    struct all_are_fields
        : public mp::all_true<is_field<T>::value...>
        { };

    /* make */

    constexpr decltype(auto) make_field = misc::make_generic_predicate<__impl::field_builder> { };

}
end_namespace_cpphibernate_schema