// Kyoei: TextureCache members, included inside the class by the adapted texture_cache.h.
// SPDX-License-Identifier: GPL-2.0-or-later
public:
    /// Images that start exactly at `cpu_addr`. Every image is registered in the page table
    /// bucket of each page it covers, so only the first page's bucket can hold one: the same
    /// result as ForEachImageInRegion plus an address check, without walking the whole range
    /// (shadPS4 145932bf #5240, backported as BP02 by the inFamous port).
    template <typename Func>
    void KyoeiForEachImageWithAddress(VAddr cpu_addr, Func&& func) {
        const auto it = page_table.find(cpu_addr >> Traits::PageBits);
        if (it == nullptr) {
            return;
        }
        for (const ImageId image_id : *it) {
            Image& image = slot_images[image_id];
            if (image.info.guest_address == cpu_addr) {
                func(image_id, image);
            }
        }
    }
