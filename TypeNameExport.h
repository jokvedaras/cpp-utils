#ifndef TYPENAMEEXPORT_H_
#define TYPENAMEEXPORT_H_

namespace util
{
   // primary template definition
   template <typename T>
   struct TypeNameExport { };
}

// forward declare of template specialization (call from header)
#define EXPORT_TYPE_NAME_F_DEC(X)                            \
   namespace util                                            \
   {                                                         \
      template <>                                            \
      struct TypeNameExport<X> { static const char *name; }; \
   }

// explicit specialization (call from cpp)
#define EXPORT_TYPE_NAME(X) \
   const char* util::TypeNameExport<X>::name = #X;

#endif /* TYPENAMEEXPORT_H_ */
