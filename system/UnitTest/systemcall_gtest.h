#ifndef SYSTEMCALL_GTEST_H_
#define SYSTEMCALL_GTEST_H_

#include "SystemCall.h"

TEST( SystemCall, All )
{
   SystemCall sys_call( "ls -la" );

   const std::string &result = sys_call.execute();
   EXPECT_GT( result.length(), 0 );

   const std::string &result2 = sys_call.execute( "ls -la /" );
   EXPECT_GT( result2.length(), 0 );

   EXPECT_GT( sys_call.getLastResult().length(), 0 );
}


#endif /* SYSTEMCALL_GTEST_H_ */
