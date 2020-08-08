/*
 *  Software MMU support
 *
 * Generate helpers used by TCG for qemu_ld/st ops and code load
 * functions.
 *
 * Included from target op helpers and exec.c.
 *
 *  Copyright (c) 2003 Fabrice Bellard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */


#define LOW_ADDR 0x30000000
#define HIGH_ADDR 0x8cabf440



#if DATA_SIZE == 8
#define SUFFIX q
#define LSUFFIX q
#define SDATA_TYPE  int64_t
#define DATA_TYPE  uint64_t
#elif DATA_SIZE == 4
#define SUFFIX l
#define LSUFFIX l
#define SDATA_TYPE  int32_t
#define DATA_TYPE  uint32_t
#elif DATA_SIZE == 2
#define SUFFIX w
#define LSUFFIX uw
#define SDATA_TYPE  int16_t
#define DATA_TYPE  uint16_t
#elif DATA_SIZE == 1
#define SUFFIX b
#define LSUFFIX ub
#define SDATA_TYPE  int8_t
#define DATA_TYPE  uint8_t
#else
#error unsupported data size
#endif


/* For the benefit of TCG generated code, we want to avoid the complication
   of ABI-specific return type promotion and always return a value extended
   to the register size of the host.  This is tcg_target_long, except in the
   case of a 32-bit host and 64-bit data, and for that we always have
   uint64_t.  Don't bother with this widened value for SOFTMMU_CODE_ACCESS.  */
#if defined(SOFTMMU_CODE_ACCESS) || DATA_SIZE == 8
# define WORD_TYPE  DATA_TYPE
# define USUFFIX    SUFFIX
#else
# define WORD_TYPE  tcg_target_ulong
# define USUFFIX    glue(u, SUFFIX)
# define SSUFFIX    glue(s, SUFFIX)
#endif

#ifdef SOFTMMU_CODE_ACCESS
#define READ_ACCESS_TYPE MMU_INST_FETCH
#define ADDR_READ addr_code
#else
#define READ_ACCESS_TYPE MMU_DATA_LOAD
#define ADDR_READ addr_read
#endif

#if DATA_SIZE == 8
# define BSWAP(X)  bswap64(X)
#elif DATA_SIZE == 4
# define BSWAP(X)  bswap32(X)
#elif DATA_SIZE == 2
# define BSWAP(X)  bswap16(X)
#else
# define BSWAP(X)  (X)
#endif

#if DATA_SIZE == 1
# define helper_le_ld_name  glue(glue(helper_ret_ld, USUFFIX), MMUSUFFIX)
# define helper_be_ld_name  helper_le_ld_name
# define helper_le_lds_name glue(glue(helper_ret_ld, SSUFFIX), MMUSUFFIX)
# define helper_be_lds_name helper_le_lds_name
# define helper_le_st_name  glue(glue(helper_ret_st, SUFFIX), MMUSUFFIX)
# define helper_be_st_name  helper_le_st_name
#else
# define helper_le_ld_name  glue(glue(helper_le_ld, USUFFIX), MMUSUFFIX)
# define helper_be_ld_name  glue(glue(helper_be_ld, USUFFIX), MMUSUFFIX)
# define helper_le_lds_name glue(glue(helper_le_ld, SSUFFIX), MMUSUFFIX)
# define helper_be_lds_name glue(glue(helper_be_ld, SSUFFIX), MMUSUFFIX)
# define helper_le_st_name  glue(glue(helper_le_st, SUFFIX), MMUSUFFIX)
# define helper_be_st_name  glue(glue(helper_be_st, SUFFIX), MMUSUFFIX)
#endif


#ifndef DRAM_GLOBLE_DEF
#define DRAM_GLOBLE_DEF
//#define LINUXC_EN
#define DEB_GVA_EN
#define IS_HOOK_ADDR    0x01
#define IS_HOOK_TLB     0x10
#include "zx_utils/zx_linklist.h"
#include "exec/exec-all.h"
extern zx_plist_t zx_g_hook_ad_list;
extern uint64_t cr3_mark;
static uint64_t dram_cnt = 0;
static uint64_t write_cnts = 0;

uint8_t inline is_hooked_addr(uint64_t addr)
{
    uint8_t flag = 0;

#ifdef EN_RANGE_ADDR
    if(addr >= LOW_ADDR && addr <= HIGH_ADDR) {
        flag |= IS_HOOK_ADDR;
    } else if (((addr & ~0xFFFUL) == ((uint64_t)LOW_ADDR & ~0xFFFUL)) ||
               ((addr & ~0xFFFUL) == ((uint64_t)HIGH_ADDR & ~0xFFFUL))) {
        flag |= IS_HOOK_TLB;
    }

#else
    if(!zx_g_hook_ad_list) {
        return 0;
    }

    zx_plist_t p = zx_g_hook_ad_list;

    while(p) {
        if((addr >= p->data.addr_low) && (addr <= p->data.addr_high)) {
            flag |= IS_HOOK_ADDR;
        } else if (((addr & ~0xFFFUL) == (p->data.addr_low & ~0xFFFUL)) ||
                   ((addr & ~0xFFFUL) == (p->data.addr_high & ~0xFFFUL))) {
            flag |= IS_HOOK_TLB;
        }

        p = p->next;
    }

    if(IS_HOOK_ADDR == flag) {
        flag |= IS_HOOK_TLB;
    }
#endif

    return flag;

    //return (((get_mmuidx(oi) == MMU_USER_IDX) && cr3 == cr3_mark)
    //     && ((addr >= 0x6c2800 && addr < (0x6c2800 + 40000 * 8))
    //     || (addr >= 0x710a00 && addr < (0x710a00 + 40000 * 8))
    //     || (addr >= 0x75ec00 && addr < (0x75ec00 + 40000 * 8))));
//    if(addr >= LOW_ADDR && addr <= HIGH_ADDR) {
//        flag |= IS_HOOK_ADDR;
//        flag |= IS_HOOK_TLB;
//    }
//    return flag;
}

uint64_t hook_load_to_tb(uint64_t addr, uint8_t size, uint64_t cmp_data)
{
    uint64_t res = 0;

    return res;
}

void hook_store_to_tb(uint64_t addr, uint8_t size, uint64_t val)
{

}
#endif




#ifndef SOFTMMU_CODE_ACCESS
static inline DATA_TYPE glue(io_read, SUFFIX)(CPUArchState *env,
                                              size_t mmu_idx, size_t index,
                                              target_ulong addr,
                                              uintptr_t retaddr,
                                              bool recheck,
                                              MMUAccessType access_type)
{
    CPUIOTLBEntry *iotlbentry = &env->iotlb[mmu_idx][index];
    return io_readx(env, iotlbentry, mmu_idx, addr, retaddr, recheck,
                    access_type, DATA_SIZE);
}
#endif

WORD_TYPE helper_le_ld_name(CPUArchState *env, target_ulong addr,
                            TCGMemOpIdx oi, uintptr_t retaddr)
{

    //enoch add begin
    uint8_t tlb_filled_flag = 0;
    uint64_t gpa = 0;
    uint64_t tlb_mask = 0xFFF;
    //enoch add end


    uintptr_t mmu_idx = get_mmuidx(oi);
    uintptr_t index = tlb_index(env, mmu_idx, addr);
    CPUTLBEntry *entry = tlb_entry(env, mmu_idx, addr);
    target_ulong tlb_addr = entry->ADDR_READ;
    unsigned a_bits = get_alignment_bits(get_memop(oi));
    uintptr_t haddr;
    DATA_TYPE res;

    if (addr & ((1 << a_bits) - 1)) {
        cpu_unaligned_access(ENV_GET_CPU(env), addr, READ_ACCESS_TYPE,
                             mmu_idx, retaddr);
    }

    /* If the TLB entry is for a different page, reload and try again.  */
    if (!tlb_hit(tlb_addr, addr)) {
        if (!VICTIM_TLB_HIT(ADDR_READ, addr)) {
            tlb_fill(ENV_GET_CPU(env), addr, DATA_SIZE, READ_ACCESS_TYPE,
            //enoch modify begin
                     //mmu_idx, retaddr);
                     mmu_idx, retaddr, &gpa);
            //enoch modify end
            index = tlb_index(env, mmu_idx, addr);
            entry = tlb_entry(env, mmu_idx, addr);

            //enoch add begin
            if(is_hooked_addr(addr) & IS_HOOK_TLB) {
                tlb_filled_flag = 1;
            }
            //enoch add end
        }
        tlb_addr = entry->ADDR_READ;
    }

    /* Handle an IO access.  */
    if (unlikely(tlb_addr & ~TARGET_PAGE_MASK)) {
        if ((addr & (DATA_SIZE - 1)) != 0) {
            goto do_unaligned_access;
        }

        /* ??? Note that the io helpers always read data in the target
           byte ordering.  We should push the LE/BE request down into io.  */
        res = glue(io_read, SUFFIX)(env, mmu_idx, index, addr, retaddr,
                                    tlb_addr & TLB_RECHECK,
                                    READ_ACCESS_TYPE);
        res = TGT_LE(res);

        //enoch add begin
        if(tlb_filled_flag) {
            entry->ADDR_READ |= TLB_INVALID_MASK;
            entry->addr_write |= TLB_INVALID_MASK;
            entry->addr_code |= TLB_INVALID_MASK;
        }
        if(is_hooked_addr(addr) & IS_HOOK_ADDR) {
            printf("enochttss11, addr=%0lx\n", addr);
        }
        //enoch add end


        return res;
    }

    /* Handle slow unaligned access (it spans two pages or IO).  */
    if (DATA_SIZE > 1
        && unlikely((addr & ~TARGET_PAGE_MASK) + DATA_SIZE - 1
                    >= TARGET_PAGE_SIZE)) {
        target_ulong addr1, addr2;
        DATA_TYPE res1, res2;
        unsigned shift;
    do_unaligned_access:
        addr1 = addr & ~(DATA_SIZE - 1);
        addr2 = addr1 + DATA_SIZE;
        res1 = helper_le_ld_name(env, addr1, oi, retaddr);
        res2 = helper_le_ld_name(env, addr2, oi, retaddr);
        shift = (addr & (DATA_SIZE - 1)) * 8;

        /* Little-endian combine.  */
        res = (res1 >> shift) | (res2 << ((DATA_SIZE * 8) - shift));

        //enoch add begin
        if(tlb_filled_flag) {
            entry->ADDR_READ |= TLB_INVALID_MASK;
            entry->addr_write |= TLB_INVALID_MASK;
            entry->addr_code |= TLB_INVALID_MASK;
        }
        if(is_hooked_addr(addr) & IS_HOOK_ADDR) {
            printf("enochttss22, addr=%0lx\n", addr);
        }
        //enoch add end


        return res;
    }

    haddr = addr + entry->addend;
#if DATA_SIZE == 1
    res = glue(glue(ld, LSUFFIX), _p)((uint8_t *)haddr);
#else
    res = glue(glue(ld, LSUFFIX), _le_p)((uint8_t *)haddr);
#endif

    //enoch add begin
    if(tlb_filled_flag) {
        entry->ADDR_READ |= TLB_INVALID_MASK;
        entry->addr_write |= TLB_INVALID_MASK;
        entry->addr_code |= TLB_INVALID_MASK;
    }

    if((is_hooked_addr(addr) & IS_HOOK_ADDR)) {
        X86CPU *cpu = x86_env_get_cpu(env);

        uint8_t src;
        src = cpu->core_id;
        //printf("READ socketid=%=%lx,coreid=%lx.\n", cpu->socket_id, cpu->core_id);

        gpa += (addr & tlb_mask);

//#ifdef DEBUG_SHOW_APP_REQ
        printf("[socket=%d,core=%d]APP READ: gva=%0lx, gpa=%0lx, size=%d.\n",
                    cpu->socket_id, cpu->core_id, addr, gpa, DATA_SIZE);
//#endif

#ifdef EN_HOOK_TO_TB
#ifdef DEB_GVA_EN
       res = hook_load_to_tb(addr, DATA_SIZE, res, src);
#else
       res = hook_load_to_tb(gpa, DATA_SIZE, res, src);
#endif
#endif
    }
    //enoch add end




    return res;
}

#if DATA_SIZE > 1
WORD_TYPE helper_be_ld_name(CPUArchState *env, target_ulong addr,
                            TCGMemOpIdx oi, uintptr_t retaddr)
{
    uint64_t gpa = 0; //enoch add

    uintptr_t mmu_idx = get_mmuidx(oi);
    uintptr_t index = tlb_index(env, mmu_idx, addr);
    CPUTLBEntry *entry = tlb_entry(env, mmu_idx, addr);
    target_ulong tlb_addr = entry->ADDR_READ;
    unsigned a_bits = get_alignment_bits(get_memop(oi));
    uintptr_t haddr;
    DATA_TYPE res;

    if (addr & ((1 << a_bits) - 1)) {
        cpu_unaligned_access(ENV_GET_CPU(env), addr, READ_ACCESS_TYPE,
                             mmu_idx, retaddr);
    }

    /* If the TLB entry is for a different page, reload and try again.  */
    if (!tlb_hit(tlb_addr, addr)) {
        if (!VICTIM_TLB_HIT(ADDR_READ, addr)) {
            tlb_fill(ENV_GET_CPU(env), addr, DATA_SIZE, READ_ACCESS_TYPE,
            //enoch modify begin
                     //mmu_idx, retaddr);
                     mmu_idx, retaddr, &gpa);
            //enoch modify end
            index = tlb_index(env, mmu_idx, addr);
            entry = tlb_entry(env, mmu_idx, addr);
        }
        tlb_addr = entry->ADDR_READ;
    }

    /* Handle an IO access.  */
    if (unlikely(tlb_addr & ~TARGET_PAGE_MASK)) {
        if ((addr & (DATA_SIZE - 1)) != 0) {
            goto do_unaligned_access;
        }

        /* ??? Note that the io helpers always read data in the target
           byte ordering.  We should push the LE/BE request down into io.  */
        res = glue(io_read, SUFFIX)(env, mmu_idx, index, addr, retaddr,
                                    tlb_addr & TLB_RECHECK,
                                    READ_ACCESS_TYPE);
        res = TGT_BE(res);
        return res;
    }

    /* Handle slow unaligned access (it spans two pages or IO).  */
    if (DATA_SIZE > 1
        && unlikely((addr & ~TARGET_PAGE_MASK) + DATA_SIZE - 1
                    >= TARGET_PAGE_SIZE)) {
        target_ulong addr1, addr2;
        DATA_TYPE res1, res2;
        unsigned shift;
    do_unaligned_access:
        addr1 = addr & ~(DATA_SIZE - 1);
        addr2 = addr1 + DATA_SIZE;
        res1 = helper_be_ld_name(env, addr1, oi, retaddr);
        res2 = helper_be_ld_name(env, addr2, oi, retaddr);
        shift = (addr & (DATA_SIZE - 1)) * 8;

        /* Big-endian combine.  */
        res = (res1 << shift) | (res2 >> ((DATA_SIZE * 8) - shift));
        return res;
    }

    haddr = addr + entry->addend;
    res = glue(glue(ld, LSUFFIX), _be_p)((uint8_t *)haddr);
    return res;
}
#endif /* DATA_SIZE > 1 */

#ifndef SOFTMMU_CODE_ACCESS

/* Provide signed versions of the load routines as well.  We can of course
   avoid this for 64-bit data, or for 32-bit data on 32-bit host.  */
#if DATA_SIZE * 8 < TCG_TARGET_REG_BITS
WORD_TYPE helper_le_lds_name(CPUArchState *env, target_ulong addr,
                             TCGMemOpIdx oi, uintptr_t retaddr)
{
    return (SDATA_TYPE)helper_le_ld_name(env, addr, oi, retaddr);
}

# if DATA_SIZE > 1
WORD_TYPE helper_be_lds_name(CPUArchState *env, target_ulong addr,
                             TCGMemOpIdx oi, uintptr_t retaddr)
{
    return (SDATA_TYPE)helper_be_ld_name(env, addr, oi, retaddr);
}
# endif
#endif

static inline void glue(io_write, SUFFIX)(CPUArchState *env,
                                          size_t mmu_idx, size_t index,
                                          DATA_TYPE val,
                                          target_ulong addr,
                                          uintptr_t retaddr,
                                          bool recheck)
{
    CPUIOTLBEntry *iotlbentry = &env->iotlb[mmu_idx][index];
    return io_writex(env, iotlbentry, mmu_idx, val, addr, retaddr,
                     recheck, DATA_SIZE);
}

void helper_le_st_name(CPUArchState *env, target_ulong addr, DATA_TYPE val,
                       TCGMemOpIdx oi, uintptr_t retaddr)
{

    //enoch add begin
    uint8_t tlb1_filled_flag = 0;
    uint8_t tlb2_filled_flag = 0;
    uint64_t gpa = 0;
    uint64_t tlb_mask = 0xFFF;
    X86CPU *cpu = x86_env_get_cpu(env);
    //enoch add end


    uintptr_t mmu_idx = get_mmuidx(oi);
    uintptr_t index = tlb_index(env, mmu_idx, addr);
    CPUTLBEntry *entry = tlb_entry(env, mmu_idx, addr);
    target_ulong tlb_addr = tlb_addr_write(entry);
    unsigned a_bits = get_alignment_bits(get_memop(oi));
    uintptr_t haddr;

    if (addr & ((1 << a_bits) - 1)) {
        cpu_unaligned_access(ENV_GET_CPU(env), addr, MMU_DATA_STORE,
                             mmu_idx, retaddr);
    }

    /* If the TLB entry is for a different page, reload and try again.  */
    if (!tlb_hit(tlb_addr, addr)) {
        if (!VICTIM_TLB_HIT(addr_write, addr)) {
            tlb_fill(ENV_GET_CPU(env), addr, DATA_SIZE, MMU_DATA_STORE,
            //enoch modify begin
                     //mmu_idx, retaddr);
                     mmu_idx, retaddr, &gpa);
            if(is_hooked_addr(addr) & IS_HOOK_TLB) {
                tlb1_filled_flag = 1;
            }
            //enoch modify end

            index = tlb_index(env, mmu_idx, addr);
            entry = tlb_entry(env, mmu_idx, addr);
        }
        tlb_addr = tlb_addr_write(entry) & ~TLB_INVALID_MASK;
    }

    /* Handle an IO access.  */
    if (unlikely(tlb_addr & ~TARGET_PAGE_MASK)) {
        if ((addr & (DATA_SIZE - 1)) != 0) {
            goto do_unaligned_access;
        }

        /* ??? Note that the io helpers always read data in the target
           byte ordering.  We should push the LE/BE request down into io.  */
        val = TGT_LE(val);
        glue(io_write, SUFFIX)(env, mmu_idx, index, val, addr,
                               retaddr, tlb_addr & TLB_RECHECK);



        //enoch add begin
        if(tlb1_filled_flag) {
            entry->addr_write |= TLB_INVALID_MASK;
            entry->addr_code |= TLB_INVALID_MASK;
            entry->ADDR_READ |= TLB_INVALID_MASK;
        }
        if(is_hooked_addr(addr) & IS_HOOK_ADDR) {
            printf("enochttss33, addr=%0lx\n", addr);
        }
        //enoch add end


        return;
    }

    /* Handle slow unaligned access (it spans two pages or IO).  */
    if (DATA_SIZE > 1
        && unlikely((addr & ~TARGET_PAGE_MASK) + DATA_SIZE - 1
                     >= TARGET_PAGE_SIZE)) {
        int i;
        target_ulong page2;
        CPUTLBEntry *entry2;
    do_unaligned_access:
        /* Ensure the second page is in the TLB.  Note that the first page
           is already guaranteed to be filled, and that the second page
           cannot evict the first.  */
        page2 = (addr + DATA_SIZE) & TARGET_PAGE_MASK;
        entry2 = tlb_entry(env, mmu_idx, page2);
        if (!tlb_hit_page(tlb_addr_write(entry2), page2)
            && !VICTIM_TLB_HIT(addr_write, page2)) {
            tlb_fill(ENV_GET_CPU(env), page2, DATA_SIZE, MMU_DATA_STORE,
            //enoch modify begin
                     //mmu_idx, retaddr);
                     mmu_idx, retaddr, &gpa);
            if(is_hooked_addr(addr) & IS_HOOK_TLB) {
                tlb2_filled_flag = 1;
            }
            //enoch modify end
        }

        /* XXX: not efficient, but simple.  */
        /* This loop must go in the forward direction to avoid issues
           with self-modifying code in Windows 64-bit.  */
        for (i = 0; i < DATA_SIZE; ++i) {
            /* Little-endian extract.  */
            uint8_t val8 = val >> (i * 8);
            glue(helper_ret_stb, MMUSUFFIX)(env, addr + i, val8,
                                            oi, retaddr);
        }
        //enoch add begin
           if(tlb1_filled_flag) {
               entry->addr_write |= TLB_INVALID_MASK;
               entry->addr_code |= TLB_INVALID_MASK;
               entry->ADDR_READ |= TLB_INVALID_MASK;
           }
           if(tlb2_filled_flag) {
               entry->addr_write |= TLB_INVALID_MASK;
               entry->addr_code |= TLB_INVALID_MASK;
               entry->ADDR_READ |= TLB_INVALID_MASK;
           }

           if(is_hooked_addr(addr) & IS_HOOK_ADDR) {
               printf("enochttss44, addr=%0lx\n", addr);
           }
           //enoch add end

        return;
    }

    haddr = addr + entry->addend;
#if DATA_SIZE == 1
    glue(glue(st, SUFFIX), _p)((uint8_t *)haddr, val);
#else
    glue(glue(st, SUFFIX), _le_p)((uint8_t *)haddr, val);
#endif


    if(tlb1_filled_flag) {
        entry->addr_write |= TLB_INVALID_MASK;
        entry->addr_code |= TLB_INVALID_MASK;
        entry->ADDR_READ |= TLB_INVALID_MASK;
    }
    if(tlb2_filled_flag) {
        entry->addr_write |= TLB_INVALID_MASK;
        entry->addr_code |= TLB_INVALID_MASK;
        entry->ADDR_READ |= TLB_INVALID_MASK;
    }

    if(is_hooked_addr(addr) & IS_HOOK_ADDR) {
        uint8_t src;
        src = cpu->core_id;
        gpa += (addr & tlb_mask);

//#ifdef DEBUG_SHOW_APP_REQ
        printf("[socket=%d,core=%d]APP WRITE: gva=%0lx, gpa=%0lx, size=%d, data=%0lx.\n",
                    cpu->socket_id, cpu->core_id, addr, gpa, DATA_SIZE, val);
//#endif

#ifdef EN_HOOK_TO_TB
#ifdef DEB_GVA_EN
        hook_store_to_tb(addr, DATA_SIZE, val, src);
#else
        hook_store_to_tb(gpa, DATA_SIZE, val, src);
#endif
#endif
    }
    //enoch add end

}

#if DATA_SIZE > 1
void helper_be_st_name(CPUArchState *env, target_ulong addr, DATA_TYPE val,
                       TCGMemOpIdx oi, uintptr_t retaddr)
{
    uint64_t gpa = 0; //enoch add

    uintptr_t mmu_idx = get_mmuidx(oi);
    uintptr_t index = tlb_index(env, mmu_idx, addr);
    CPUTLBEntry *entry = tlb_entry(env, mmu_idx, addr);
    target_ulong tlb_addr = tlb_addr_write(entry);
    unsigned a_bits = get_alignment_bits(get_memop(oi));
    uintptr_t haddr;

    if (addr & ((1 << a_bits) - 1)) {
        cpu_unaligned_access(ENV_GET_CPU(env), addr, MMU_DATA_STORE,
                             mmu_idx, retaddr);
    }

    /* If the TLB entry is for a different page, reload and try again.  */
    if (!tlb_hit(tlb_addr, addr)) {
        if (!VICTIM_TLB_HIT(addr_write, addr)) {
            tlb_fill(ENV_GET_CPU(env), addr, DATA_SIZE, MMU_DATA_STORE,
            //enoch moddify begin
                     mmu_idx, retaddr, &gpa);
            //enoch moddify end
            index = tlb_index(env, mmu_idx, addr);
            entry = tlb_entry(env, mmu_idx, addr);
        }
        tlb_addr = tlb_addr_write(entry) & ~TLB_INVALID_MASK;
    }

    /* Handle an IO access.  */
    if (unlikely(tlb_addr & ~TARGET_PAGE_MASK)) {
        if ((addr & (DATA_SIZE - 1)) != 0) {
            goto do_unaligned_access;
        }

        /* ??? Note that the io helpers always read data in the target
           byte ordering.  We should push the LE/BE request down into io.  */
        val = TGT_BE(val);
        glue(io_write, SUFFIX)(env, mmu_idx, index, val, addr, retaddr,
                               tlb_addr & TLB_RECHECK);
        return;
    }

    /* Handle slow unaligned access (it spans two pages or IO).  */
    if (DATA_SIZE > 1
        && unlikely((addr & ~TARGET_PAGE_MASK) + DATA_SIZE - 1
                     >= TARGET_PAGE_SIZE)) {
        int i;
        target_ulong page2;
        CPUTLBEntry *entry2;
    do_unaligned_access:
        /* Ensure the second page is in the TLB.  Note that the first page
           is already guaranteed to be filled, and that the second page
           cannot evict the first.  */
        page2 = (addr + DATA_SIZE) & TARGET_PAGE_MASK;
        entry2 = tlb_entry(env, mmu_idx, page2);
        if (!tlb_hit_page(tlb_addr_write(entry2), page2)
            && !VICTIM_TLB_HIT(addr_write, page2)) {
            tlb_fill(ENV_GET_CPU(env), page2, DATA_SIZE, MMU_DATA_STORE,
            //enoch modify begin
                     //mmu_idx, retaddr);
                     mmu_idx, retaddr, &gpa);
            //enoch modify end
        }


        /* XXX: not efficient, but simple */
        /* This loop must go in the forward direction to avoid issues
           with self-modifying code.  */
        for (i = 0; i < DATA_SIZE; ++i) {
            /* Big-endian extract.  */
            uint8_t val8 = val >> (((DATA_SIZE - 1) * 8) - (i * 8));
            glue(helper_ret_stb, MMUSUFFIX)(env, addr + i, val8,
                                            oi, retaddr);
        }
        return;
    }

    haddr = addr + entry->addend;
    glue(glue(st, SUFFIX), _be_p)((uint8_t *)haddr, val);
}
#endif /* DATA_SIZE > 1 */
#endif /* !defined(SOFTMMU_CODE_ACCESS) */

#undef READ_ACCESS_TYPE
#undef DATA_TYPE
#undef SUFFIX
#undef LSUFFIX
#undef DATA_SIZE
#undef ADDR_READ
#undef WORD_TYPE
#undef SDATA_TYPE
#undef USUFFIX
#undef SSUFFIX
#undef BSWAP
#undef helper_le_ld_name
#undef helper_be_ld_name
#undef helper_le_lds_name
#undef helper_be_lds_name
#undef helper_le_st_name
#undef helper_be_st_name
